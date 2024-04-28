#include <logika/protocols/m4/archive4m.h>

/// @cond
#include <stdexcept>
/// @endcond

namespace logika
{

namespace protocols
{

ByteVector FastLzImpl::Decompress( const ByteVector& input, uint32_t offset, uint32_t length )
{
    constexpr uint32_t maxOutLength = 32768;

    if ( 0 == length || input.size() + offset < length )
    {
        return ByteVector{};
    }

    int level = ( input.at( 0 ) >> 5 ) + 1;
    if ( level != 1 )
    {
        throw std::runtime_error{ "Only FLZ level1 decompression supported" };
    }

    ByteVector out( maxOutLength );

    uint32_t ip         = offset;                       // input pointer
    uint32_t ipLimit    = ip + length;
    uint32_t op         = 0;                            // output pointer
    uint32_t opLimit    = maxOutLength;
    uint32_t control    = ( input.at( ip++ ) ) & 31;
    bool loop           = true;

    do
    {
        int32_t ref = op;
        uint32_t len = control >> 5;
        uint32_t ofs = ( control & 31 ) << 8;

        if ( control >= 32 )
        {
            --len;
            ref -= ofs;
            if ( len == 7 - 1 )
            {
                len += input.at( ip++ );
            }
            ref -= input.at( ip++ );
            if ( op + len + 3 > opLimit
                || ref < 1 )
            {
                return ByteVector{};
            }
            if ( ip < ipLimit )
            {
                control = input.at( ip++ );
            }
            else
            {
                loop = false;
            }

            if ( static_cast< uint32_t >( ref ) == op )
            {
                /// optimize copy for a run
                ByteType b = out[ ref - 1 ];
                out[ op++ ] = b;
                out[ op++ ] = b;
                out[ op++ ] = b;
                for ( ; len > 0; --len )
                {
                    out[ op++ ] = b;
                }
            }
            else
            {
                /// copy from reference
                --ref;
                out[ op++ ] = out[ ref++ ];
                out[ op++ ] = out[ ref++ ];
                out[ op++ ] = out[ ref++ ];
                /// copy a byte, so that now it's word aligned
                if ( ( len & 1 ) > 0 )
                {
                    out[ op++ ] = out[ ref++ ];
                    --len;
                }
                /// copy 16-bit at once
                uint32_t p = op;
                op += len;
                uint32_t q = ref;
                for ( len >>= 1; len > 4; len -= 4 )
                {
                    out[ q++ ] = out[ p++ ]; out[ q++ ] = out[ p++ ];
                    out[ q++ ] = out[ p++ ]; out[ q++ ] = out[ p++ ];
                    out[ q++ ] = out[ p++ ]; out[ q++ ] = out[ p++ ];
                    out[ q++ ] = out[ p++ ]; out[ q++ ] = out[ p++ ];
                }
                for ( ; len > 0; --len )
                {
                    out[ q++ ] = out[ p++ ]; out[ q++ ] = out[ p++ ];
                }
            }
        }
        else
        {
            ++control;
            if (   op + control > opLimit
                || ip + control > ipLimit )
            {
                return ByteVector{};
            }
            out[ op++ ] = input.at( ip++ );
            for ( --control; control > 0; --control )
            {
                out[ op++ ] = input.at( ip++ );
            }
            loop = ( ip < ipLimit );
            if ( loop )
            {
                control = input.at( ip++ );
            }
        }
    } while ( loop );

    out.resize( op );
    return out;
} // Decompress

} // namespace protocols

} // namespace logika
