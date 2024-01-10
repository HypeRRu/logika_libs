#include <connections/connection.h>
#include <iostream>

int main()
{
    logika::connections::Connection con;
    std::cout << con.GetConnectionType() << '\n';
    return 0;
}
