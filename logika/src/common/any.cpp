#include <logika/common/any.hpp>

namespace logika
{

Any::Any( Any&& other )
{
    std::swap( holder_, other.holder_ );
} // Any( Any&& other )


Any& Any::operator= ( Any&& other )
{
    std::swap( holder_, other.holder_ );
    return *this;
} // operator= ( Any&& other )


bool Any::Empty() const
{
    return holder_.get() == nullptr;
} // Empty

} // namespace logika
