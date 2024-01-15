#include <logika/connections/connection.h>
#include <iostream>

int main()
{
    logika::connections::Connection con{ "localhost", 0 };
    std::cout << con.GetConnectionType() << '\n';
    return 0;
}
