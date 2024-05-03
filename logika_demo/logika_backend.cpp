#include "logika_backend.h"

#include <logika/storage/storage.hpp>
#include <logika/log/defines.h>
#include <logika/resources/loader.hpp>

#include <logika/connections/serial/serial_port_connection.h>
#include <logika/connections/network/tcp_connection.h>
#include <logika/connections/network/udp_connection.h>
#include <logika/meters/data_tag.h>

#if defined( LOGIKA_USE_RESOURCE_CONVERTERS )
#   include <logika/meters/converters/archive_type_converter.h>
#   include <logika/meters/converters/device_converter.h>
#   include <logika/meters/converters/channel_converter.h>
#   if defined( LOGIKA_USE_METERS4M )
#       include <logika/meters/converters/m4_tag_converter.h>
#       include <logika/meters/converters/m4_archive_converter.h>
#       include <logika/meters/converters/m4_archive_field_converter.h>
#   endif // defined( LOGIKA_USE_METERS4M )
#   if defined( LOGIKA_USE_METERS4L )
#       include <logika/meters/converters/l4_tag_converter.h>
#       include <logika/meters/converters/l4_archive_converter.h>
#       include <logika/meters/converters/l4_archive_field_converter.h>
#   endif // defined( LOGIKA_USE_METERS4L )
#endif // defined( LOGIKA_USE_RESOURCE_CONVERTERS )

#include "ui_mainwindow.h"
#include <QStandardItemModel>


LogikaBackend& LogikaBackend::Instance()
{
    static LogikaBackend instance;
    return instance;
} // Instance


void LogikaBackend::InitLogger()
{
    logika::log::Logger& logger = logika::log::Logger::Instance();
    logger.SetLogType( logika::log::LogType::LogConsole | logika::log::LogType::LogFile );
    logger.SetLogLevel( logika::log::LogLevel::LogAll );
    logger.SetLogFile( "/home/hyper/log.txt", true );
} // InitLogger


void LogikaBackend::ConnectTcp( const std::string& address, uint16_t port, uint64_t timeout )
{
    connection = std::make_shared< logika::connections::TcpConnection >( address, port, timeout );
    connection->Open();
    if ( !connection->IsConnected() )
    {
        connection = nullptr;
    }
} // ConnectTcp


void LogikaBackend::ConnectUdp( const std::string& address, uint16_t port, uint64_t timeout )
{
    connection = std::make_shared< logika::connections::UdpConnection >( address, port, timeout );
    connection->Open();
    if ( !connection->IsConnected() )
    {
        connection = nullptr;
    }
} // ConnectUdp


void LogikaBackend::ConnectSerial( const std::string& address, uint64_t timeout )
{
    connection = std::make_shared< logika::connections::SerialPortConnection >( address, timeout );
    connection->Open();
    if ( !connection->IsConnected() )
    {
        connection = nullptr;
    }
} // ConnectSerial


void LogikaBackend::SetProtocolAndDetectMeter()
{
    auto sKeeper = logika::storage::StorageKeeper::Instance();
    if ( !connection )
    {
        ui->meterParams->setText( "Connection not set" );
        protocol = nullptr;
        meter = nullptr;
        ResetMeterTable();
        return;
    }
    protocol = std::make_shared< logika::protocols::M4::M4Protocol >( sKeeper );
    protocol->SetConnection( connection );

    logika::ByteVector meterDump{};
    logika::LocString meterModel{};
    try {
        meter = logika::protocols::Protocol::DetectM4(
            protocol, sKeeper, meterDump, meterModel );
        if ( !meter )
        {
            LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Meter detection failed" ) );
            ui->meterParams->setText( "Undefined" );
        }
        else
        {
            LOG_WRITE( LOG_INFO, LOCALIZED( "Detected meter is " ) << meter->GetCaption() );
            ui->meterParams->setText( QString::fromStdWString( meter->GetCaption() ) );
        }
    }  catch ( const std::exception& e ) {
        LOG_WRITE( LOG_INFO, LOCALIZED( "exception: " ) << logika::ToLocString( e.what() ) );
        ui->meterParams->setText( QString::fromStdString( e.what() ) );
    }

    ResetMeterTable();
} // SetProtocolAndDetectMeter


void LogikaBackend::ResetMeterTable()
{
    if ( !meter )
    {
        ui->params->setEnabled( false );
        ui->archives->setEnabled( false );
        return;
    }

    ui->params->setEnabled( true );
    ui->archives->setEnabled( true );

    try
    {
        logika::TimeType devClock = protocol->GetDeviceClock( meter, nullptr, nullptr );
        LOG_WRITE( LOG_INFO, LOCALIZED( "Device time: " ) << logika::GetTimeString( devClock ) );
        ui->meterParams->setText( ui->meterParams->text() + QString( " " )
            + QString::fromStdWString( logika::GetTimeString( devClock ) ) );
    }
    catch( const std::exception& e )
    {
        LOG_WRITE( LOG_ERROR, LOCALIZED( "Exception: " ) << logika::ToLocString( e.what() ) );
    }

    const auto& tagDefs = meter->GetTagsVault()->All();
    std::vector< std::shared_ptr< logika::meters::DataTag > > tags;
    for ( auto tagDef: tagDefs )
    {
        tags.emplace_back( std::make_shared< logika::meters::DataTag >( tagDef, tagDef->GetChannelDef().start ) );
    }

//    auto wktags = meter->GetWellKnownTags();
//    size_t countTags = 0;
//    for ( auto& wktag: wktags )
//    {
//        countTags += wktag.second.size();
//    }
    QStandardItemModel* modelT = new QStandardItemModel( tags.size(), 4, ui->params );
    modelT->setHeaderData( 0, Qt::Horizontal, "Tag" );
    modelT->setHeaderData( 1, Qt::Horizontal, "Channel" );
    modelT->setHeaderData( 2, Qt::Horizontal, "Value" );
    modelT->setHeaderData( 3, Qt::Horizontal, "Units" );

    QStandardItemModel* modelA = new QStandardItemModel( meter->GetArchives().size(), 4, ui->params );
    modelA->setHeaderData( 0, Qt::Horizontal, "Archive" );
    modelA->setHeaderData( 1, Qt::Horizontal, "Type" );
    modelA->setHeaderData( 2, Qt::Horizontal, "Synchronous" );
    modelA->setHeaderData( 3, Qt::Horizontal, "Records count" );

    size_t iter = 0;
    constexpr size_t batchSize = 24;
    for ( size_t j = 0; j < tags.size() / batchSize; ++j )
    {
        try
        {
            size_t leftUnread = tags.size() - j * batchSize;
            std::vector< std::shared_ptr< logika::meters::DataTag > > batch{
                tags.begin() + j * batchSize,
                tags.begin() + j * batchSize + ( leftUnread > batchSize ? batchSize : leftUnread )
            };
            LOG_WRITE( LOG_DEBUG, LOCALIZED( "Update part of well-known tags, left " ) << leftUnread );
            LOG_WRITE( LOG_DEBUG, LOCALIZED( "Batch size " ) << batch.size() );
            protocol->UpdateTags( nullptr, nullptr, batch );
            for ( size_t i = 0; i < batch.size(); ++i )
            {
                const auto& tag = batch[ i ];
                LOG_WRITE( LOG_DEBUG, LOCALIZED( "" ) );
                LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag name: " ) << tag->GetName() );
                LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag channel: " ) << tag->GetChannel().prefix
                           << LOCALIZED( " " ) << tag->GetChannel().no );
                LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag ordinal: " ) << static_cast< uint32_t >( tag->GetOrdinal() ) );

                bool found = false;
                logika::LocStringStream valStream;
                {
                    if ( !found && !tag->HasValue() )
                    {
                        valStream << LOCALIZED( "[empty]" );
                        found = true;
                    }
                }

                {
                    logika::LocString val{};
                    if ( !found && tag->TryGetValue< logika::LocString >( val ) )
                    {
                        if ( !val.empty() )
                        {
                            for ( auto c: val )
                            {
                                if ( c < 0x20 )
                                {
                                    continue;
                                }
                                valStream << c;
                            }
                        }
                        found = true;
                    }
                }

                {
                    float val{};
                    if ( !found && tag->TryGetValue< float >( val ) )
                    {
                        valStream << val;
                        found = true;
                    }
                }

                {
                    double val{};
                    if ( !found && tag->TryGetValue< double >( val ) )
                    {
                        valStream << val;
                        found = true;
                    }
                }

                {
                    uint32_t val{};
                    if ( !found && tag->TryGetValue< uint32_t >( val ) )
                    {
                        valStream << val;
                        found = true;
                    }
                }

                {
                    logika::TimeType val{};
                    if ( !found && tag->TryGetValue< logika::TimeType >( val ) )
                    {
                        valStream << val;
                        found = true;
                    }
                }

                {
                    logika::ByteVector val{};
                    if ( !found && tag->TryGetValue< logika::ByteVector >( val ) )
                    {
                        logika::LocString valStr = LOCALIZED( "{ " );
                        for ( logika::ByteType v: val )
                        {
                            valStr += logika::ToLocString( std::to_string( static_cast< uint32_t >( v ) & 0xFF ) ) + LOCALIZED( " " );
                        }
                        valStr += LOCALIZED( "}" );
                        valStream << valStr;
                        found = true;
                    }
                }

                {
                    std::vector< size_t > val{};
                    if ( !found && tag->TryGetValue< std::vector< size_t > >( val ) )
                    {
                        logika::LocString valStr = LOCALIZED( "{ " );
                        for ( size_t v: val )
                        {
                            valStr += logika::ToLocString( std::to_string( static_cast< uint32_t >( v ) & 0xFF ) ) + LOCALIZED( " " );
                        }
                        valStr += LOCALIZED( "}" );
                        valStream << valStr;
                        found = true;
                    }
                }

                if ( !found )
                {
                    valStream << LOCALIZED( "<unknown>" );
                }

                auto tagDef4 = std::dynamic_pointer_cast< logika::meters::TagDef4 >( tag->GetDef() );

                LOG_WRITE( LOG_DEBUG, LOCALIZED( "Tag value: " ) << valStream.str() << ", iter " << iter );
                modelT->setItem( iter, 0, new QStandardItem( QString::fromStdWString( tag->GetName() ) ) );
                modelT->setItem( iter, 1, new QStandardItem( QString::fromStdWString( tag->GetChannel().prefix ) ) );
                modelT->setItem( iter, 2, new QStandardItem( QString::fromStdWString( valStream.str() ) ) );
                modelT->setItem( iter, 3, new QStandardItem( QString::fromStdWString( tagDef4 ? tagDef4->GetUnits() : LOCALIZED( "н/д" ) ) ) );
                ++iter;
            }
        }
        catch ( const std::exception& e )
        {
            LOG_WRITE( LOG_WARNING, LOCALIZED( "Update failed: " ) << logika::ToLocString( e.what() ) );
        }
    }

    ui->params->setModel(modelT);

    size_t aiter = 0;
    for ( auto archiveDef: meter->GetArchives() )
    {

        if ( !archiveDef || !archiveDef->GetArchiveType() )
        {
            continue;
        }
        auto arType = archiveDef->GetArchiveType();

        modelA->setItem( aiter, 0, new QStandardItem( QString::fromStdWString( archiveDef->GetName() ) ) );
        modelA->setItem( aiter, 1, new QStandardItem( QString::fromStdWString( arType->GetName() ) ) );
        modelA->setItem( aiter, 2, new QStandardItem( QString::fromStdString( arType->IsIntervalArchive() ? "Sync" : "Async" ) ) );
        try
        {
            if ( arType->IsIntervalArchive() )
            {
                std::shared_ptr< logika::Any > state = nullptr;
                std::shared_ptr< logika::meters::IntervalArchive > intArchive
                    = protocol->ReadIntervalArchiveDef( meter, nullptr, nullptr, arType, state );

                if ( intArchive )
                {
                    LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Reading interval archive" ) );
                    double progress = 0.0;
                    while ( protocol->ReadIntervalArchive( meter, nullptr, nullptr, intArchive,
                        logika::GetCurrentTimestamp() - 1000ull * 60 * 60 * 24 * 5,
                        logika::GetCurrentTimestamp(), state, progress ) )
                    {
                        LOG_WRITE( LOG_INFO, LOCALIZED( "Read interval archive progress is " ) << progress << LOCALIZED( "%" ) );
                    }
                    LOG_WRITE( LOG_INFO, LOCALIZED( "After read interval archive progress is " ) << progress << LOCALIZED( "%" ) );
                    modelA->setItem( aiter, 3, new QStandardItem( QString::fromStdString(
                        std::to_string( intArchive->GetDataTable()->GetRecordsList().size() ) ) )
                    );
                    ++aiter;
                    continue;
                }
                else
                {
                    LOG_WRITE_MSG( LOG_WARNING, LOCALIZED( "Unable to read interval archive def" ) );
                }
            }
            else
            {
                LOG_WRITE_MSG( LOG_DEBUG, LOCALIZED( "Reading service archive" ) );
                std::shared_ptr< logika::Any > state
                    = std::make_shared< logika::Any >( static_cast< logika::TimeType >( 0 ) );
                std::shared_ptr< logika::meters::ServiceArchive > servArchive
                    = std::make_shared< logika::meters::ServiceArchive >( meter, arType );
                double progress = 0.0;
                while ( protocol->ReadServiceArchive( meter, nullptr, nullptr, servArchive,
                    logika::GetCurrentTimestamp() - 1000ull * 60 * 60 * 24 * 1,
                    logika::GetCurrentTimestamp(), state, progress ) )
                {
                    LOG_WRITE( LOG_INFO, LOCALIZED( "Read service archive progress is " ) << progress << LOCALIZED( "%" ) );
                }
                LOG_WRITE( LOG_INFO, LOCALIZED( "After read service archive progress is " ) << progress << LOCALIZED( "%" ) );
                modelA->setItem( aiter, 3, new QStandardItem( QString::fromStdString(
                    std::to_string( servArchive->GetRecords().size() ) ) )
                );
                ++aiter;
                continue;
            }
        }
        catch ( const std::exception& e )
        {
            LOG_WRITE( LOG_WARNING, LOCALIZED( "Archive read failed: " ) << logika::ToLocString( e.what() ) );
        }
        modelA->setItem( aiter, 3, new QStandardItem( QString( "Bad" ) ) );
        ++aiter;
    }

    ui->archives->setModel(modelA);
} // ResetMeterTable


void LogikaBackend::LoadResources( const std::string& pathTo, bool silent )
{
    auto& sKeeper = logika::storage::StorageKeeper::Instance();
    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveType >();
    auto atStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveType >();
    {
        logika::resources::Loader< logika::resources::ArchiveTypes > loader;
        auto resource = loader.Load( pathTo + "/ArchiveTypes.dat" );
        if ( resource )
        {
            auto types = logika::meters::converters::ArchiveTypeConverter::Convert( *resource );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << types.size() << LOCALIZED( " ArchiveType instances" ) );
            }
            for ( auto type: types )
            {
                if ( type )
                {
                    bool res = atStorage->AddItem( type->GetName(), type );
                    if ( !silent )
                    {
                        LOG_WRITE( LOG_INFO, LOCALIZED( "Add ArchiveType '" )
                                   << type->GetName() << LOCALIZED( "' to storage: " )
                                   << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                    }
                    std::shared_ptr< logika::meters::ArchiveType > item;
                    atStorage->GetItem( type->GetName(), item );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load ArchiveTypes" ) );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::Meter >();
    auto mStorage = sKeeper.GetStorage< logika::LocString, logika::meters::Meter >();
    {
        logika::resources::Loader< logika::resources::DeviceList > loader;
        auto resource = loader.Load( pathTo + "/Devices.dat" );
        if ( resource )
        {
            auto devices = logika::meters::converters::DeviceConverter::Convert( *resource );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << devices.size() << LOCALIZED( " Device instances" ) );
            }
            for ( auto device: devices )
            {
                if ( device )
                {
                    bool res = mStorage->AddItem( device->GetCaption(), device );
                    if ( !silent )
                    {
                        LOG_WRITE( LOG_INFO, LOCALIZED( "Add Device '" )
                                   << device->GetCaption() << LOCALIZED( "' to storage: " )
                                   << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                    }
                    std::shared_ptr< logika::meters::Meter > item;
                    mStorage->GetItem( device->GetCaption(), item );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load DeviceList" ) );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ChannelDef >();
    auto cdStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ChannelDef >();
    {
        logika::resources::Loader< logika::resources::ChannelList > loader;
        auto resource = loader.Load( pathTo + "/Channels.dat" );
        if ( resource )
        {
            auto channels = logika::meters::converters::ChannelConverter::Convert( *resource, mStorage );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << channels.size() << LOCALIZED( " Channel instances" ) );
            }
            for ( auto channel: channels )
            {
                if ( channel )
                {
                    if ( !channel->meter )
                    {
                        LOG_WRITE( LOG_ERROR, LOCALIZED( "No meter instance found for " )
                                   << channel->prefix << LOCALIZED( "(" ) << channel->description << LOCALIZED( ")" ) );
                    }
                    else
                    {
                        logika::LocString label = channel->meter->GetCaption() + LOCALIZED( "." ) + channel->prefix;
                        bool res = cdStorage->AddItem( label, channel );
                        if ( !silent )
                        {
                            LOG_WRITE( LOG_INFO, LOCALIZED( "Add Channel '" )
                                       << label << LOCALIZED( "' to storage: " )
                                       << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                        }
                        std::shared_ptr< logika::meters::ChannelDef > item;
                        cdStorage->GetItem( label, item );
                    }
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load Channels" ) );
        }
    }

#if defined( LOGIKA_USE_METERS4L )
    sKeeper.CreateStorage< logika::LocString, logika::meters::TagDef4L >();
    auto l4tStorage = sKeeper.GetStorage< logika::LocString, logika::meters::TagDef4L >();
    {
        logika::resources::Loader< logika::resources::L4TagList > loader;
        auto resource = loader.Load( pathTo + "/L4Tags.dat" );
        if ( resource )
        {
            auto l4tags = logika::meters::converters::L4TagConverter::Convert( *resource, mStorage, cdStorage );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << l4tags.size() << LOCALIZED( " L4Tags instances" ) );
            }
            for ( auto tag: l4tags )
            {
                if ( tag )
                {
                    if ( !tag->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, LOCALIZED( "No meter instance found for " ) << tag->GetName() );
                    }
                    else
                    {
                        logika::LocString label = tag->GetMeter()->GetCaption() + LOCALIZED( "." ) + tag->GetName();
                        bool res = l4tStorage->AddItem( label, tag );
                        if ( !silent )
                        {
                            LOG_WRITE( LOG_INFO, LOCALIZED( "Add L4Tag '" )
                                       << label << LOCALIZED( "' to storage: " )
                                       << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                        }
                        std::shared_ptr< logika::meters::TagDef4L > item;
                        l4tStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to convert L4Tag" ) );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load L4Tags" ) );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveDef4L >();
    auto l4aStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveDef4L >();
    {
        logika::resources::Loader< logika::resources::L4ArchiveList > loader;
        auto resource = loader.Load( pathTo + "/L4Archives.dat" );
        if ( resource )
        {
            auto l4archives = logika::meters::converters::L4ArchiveConverter::Convert( *resource, mStorage, cdStorage, atStorage );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << l4archives.size() << LOCALIZED( " L4Archive instances" ) );
            }
            for ( auto archive: l4archives )
            {
                if ( archive )
                {
                    if ( !archive->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, LOCALIZED( "No meter instance found for " ) << archive->GetName() );
                    }
                    else
                    {
                        logika::LocString label = archive->GetMeter()->GetCaption() + LOCALIZED( "." ) + archive->GetArchiveType()->GetName();
                        bool res = l4aStorage->AddItem( label, archive );
                        if ( !silent )
                        {
                            LOG_WRITE( LOG_INFO, LOCALIZED( "Add L4Archive '" )
                                       << label << LOCALIZED( "' to storage: " )
                                       << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                        }
                        std::shared_ptr< logika::meters::ArchiveDef4L > item;
                        l4aStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to convert L4Archive" ) );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load L4Archives" ) );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveFieldDef4L >();
    auto l4afStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveFieldDef4L >();
    {
        logika::resources::Loader< logika::resources::L4ArchiveFieldList > loader;
        auto resource = loader.Load( pathTo + "/L4ArchiveFields.dat" );
        if ( resource )
        {
            auto l4afs = logika::meters::converters::L4ArchiveFieldConverter::Convert(
                        *resource, mStorage, l4aStorage, atStorage );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << l4afs.size() << LOCALIZED( " L4ArchiveField instances" ) );
            }
            for ( auto field: l4afs )
            {
                if ( field )
                {
                    if ( !field->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, LOCALIZED( "No meter instance found for " ) << field->GetName() );
                    }
                    else
                    {
                        logika::LocString label = field->GetMeter()->GetCaption()
                                + LOCALIZED( "." ) + field->GetArchiveType()->GetName()
                                + LOCALIZED( "." ) + field->GetName();
                        bool res = l4afStorage->AddItem( label, field );
                        if ( !silent )
                        {
                            LOG_WRITE( LOG_INFO, LOCALIZED( "Add L4ArchiveField '" )
                                       << label << LOCALIZED( "' to storage: " )
                                       << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                        }
                        std::shared_ptr< logika::meters::ArchiveFieldDef4L > item;
                        l4afStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to convert L4ArchiveField" ) );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load L4ArchiveFields" ) );
        }
    }
#endif // defined( LOGIKA_USE_METERS4L )

#if defined( LOGIKA_USE_METERS4M )
    sKeeper.CreateStorage< logika::LocString, logika::meters::TagDef4M >();
    auto m4tStorage = sKeeper.GetStorage< logika::LocString, logika::meters::TagDef4M >();
    {
        logika::resources::Loader< logika::resources::M4TagList > loader;
        auto resource = loader.Load( pathTo + "/M4Tags.dat" );
        if ( resource )
        {
            auto m4tags = logika::meters::converters::M4TagConverter::Convert( *resource, mStorage, cdStorage );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << m4tags.size() << LOCALIZED( " M4Tags instances" ) );
            }
            for ( auto tag: m4tags )
            {
                if ( tag )
                {
                    if ( !tag->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, LOCALIZED( "No meter instance found for " ) << tag->GetName() );
                    }
                    else
                    {
                        logika::LocString label = tag->GetMeter()->GetCaption() + LOCALIZED( "." ) + tag->GetName();
                        bool res = m4tStorage->AddItem( label, tag );
                        if ( !silent )
                        {
                            LOG_WRITE( LOG_INFO, LOCALIZED( "Add M4Tag '" )
                                       << label << LOCALIZED( "' to storage: " )
                                       << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                        }
                        std::shared_ptr< logika::meters::TagDef4M > item;
                        m4tStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to convert M4Tag" ) );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load M4Tags" ) );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveDef4M >();
    auto m4aStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveDef4M >();
    {
        logika::resources::Loader< logika::resources::M4ArchiveList > loader;
        auto resource = loader.Load( pathTo + "/M4Archives.dat" );
        if ( resource )
        {
            auto m4archives = logika::meters::converters::M4ArchiveConverter::Convert( *resource, mStorage, cdStorage, atStorage );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << m4archives.size() << LOCALIZED( " M4Archive instances" ) );
            }
            for ( auto archive: m4archives )
            {
                if ( archive )
                {
                    if ( !archive->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, LOCALIZED( "No meter instance found for " ) << archive->GetName() );
                    }
                    else
                    {
                        logika::LocString label = archive->GetMeter()->GetCaption() + LOCALIZED( "." ) + archive->GetArchiveType()->GetName();
                        bool res = m4aStorage->AddItem( label, archive );
                        if ( !silent )
                        {
                            LOG_WRITE( LOG_INFO, LOCALIZED( "Add M4Archive '" )
                                       << label << LOCALIZED( "' to storage: " )
                                       << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                        }
                        std::shared_ptr< logika::meters::ArchiveDef4M > item;
                        m4aStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to convert M4Archive" ) );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load M4Archives" ) );
        }
    }

    sKeeper.CreateStorage< logika::LocString, logika::meters::ArchiveFieldDef4M >();
    auto m4afStorage = sKeeper.GetStorage< logika::LocString, logika::meters::ArchiveFieldDef4M >();
    {
        logika::resources::Loader< logika::resources::M4ArchiveFieldList > loader;
        auto resource = loader.Load( pathTo + "/M4ArchiveFields.dat" );
        if ( resource )
        {
            auto m4afs = logika::meters::converters::M4ArchiveFieldConverter::Convert(
                        *resource, mStorage, m4aStorage, atStorage );
            if ( !silent )
            {
                LOG_WRITE( LOG_INFO, LOCALIZED( "Converted " ) << m4afs.size() << LOCALIZED( " M4ArchiveField instances" ) );
            }
            for ( auto field: m4afs )
            {
                if ( field )
                {
                    if ( !field->GetMeter() )
                    {
                        LOG_WRITE( LOG_ERROR, LOCALIZED( "No meter instance found for " ) << field->GetName() );
                    }
                    else
                    {
                        logika::LocString label = field->GetMeter()->GetCaption()
                                + LOCALIZED( "." ) + field->GetArchiveType()->GetName()
                                + LOCALIZED( "." ) + field->GetName();
                        bool res = m4afStorage->AddItem( label, field );
                        if ( !silent )
                        {
                            LOG_WRITE( LOG_INFO, LOCALIZED( "Add M4ArchiveField '" )
                                       << label << LOCALIZED( "' to storage: " )
                                       << ( res ? LOCALIZED( "Success" ) : LOCALIZED( "Failed" ) ) );
                        }
                        std::shared_ptr< logika::meters::ArchiveFieldDef4M > item;
                        m4afStorage->GetItem( label, item );
                    }
                }
                else
                {
                    LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to convert M4ArchiveField" ) );
                }
            }
        }
        else
        {
            LOG_WRITE( LOG_ERROR, LOCALIZED( "Unable to load M4ArchiveFields" ) );
        }
    }
#endif // defined( LOGIKA_USE_METERS4M )

    for ( const logika::LocString& devCaption: mStorage->GetKeys() )
    {
        std::shared_ptr< logika::meters::Meter > meter = nullptr;
        if ( mStorage->GetItem( devCaption, meter ) && meter )
        {
            meter->Init( sKeeper );
        }
    }
} // LoadResources
