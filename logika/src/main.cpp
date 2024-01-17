#include <logika/connections/connection.h>
#include <iostream>

int main()
{
    logika::connections::Connection con{ "localhost", 0 };
    con.Open();
    con.Write( { 'b', 'u', 'f', 'f', 'e', 'r', '\n' } );
    return 0;
}
