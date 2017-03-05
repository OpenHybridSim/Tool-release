///////////////////////////////////////////////////////////////////////////////
/// @file         psocket.c
///
/// @author       Thomas Roth <tprfh7@mst.edu>
///
/// @project      FREEDM DGI
///
/// @description  socket code for the PSCAD send and receive components
///
/// @functions
///     print_header( const char *, const char *, int )
///     print_data( const char *, const char *, const double *, int )
///     print_error( const char *, const char * )
///     print_footer( const char * )
///     itodd( int, int, int, int, char * )
///     connect_to_server( const char *, int, char * )
///     send_packet( int, const char *, const void *, int, char * )
///     receive_packet( int, void *, int, char * )
///     pscad_send_init( int *, int *, int *, int *, int *, double *, int * )
///     pscad_send_close( )
///     pscad_send( int *, int *, int *, int *, int *, double *, int * )
///     pscad_recv_init( int *, int *, int *, int *, int * )
///     pscad_recv( int *, int *, int *, int *, int *, double *, int * )
///     pscad_recv_close( )
///
/// These source code files were created at the Missouri University of Science
/// and Technology, and are intended for use in teaching or research. They may
/// be freely copied, modified and redistributed as long as modified versions
/// are clearly marked as such and this notice is not removed.
///
/// Neither the authors nor Missouri S&T make any warranty, express or implied,
/// nor assume any legal responsibility for the accuracy, completeness or
/// usefulness of these files or any information distributed with these files.
///
/// Suggested modifications or questions about these files can be directed to
/// Dr. Bruce McMillin, Department of Computer Science, Missouri University of
/// Science and Technology, Rolla, MO 65409 <ff@mst.edu>.
///////////////////////////////////////////////////////////////////////////////

#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <Winsock2.h>
#include <io.h>
 
#else
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#define PKT_HDR_SIZE    5

#define SENDLOG         "pscad_send.txt"
#define RECVLOG         "pscad_recv.txt"

//
// Convert double array to char array for sending it to the server side;
////////////////////////////////////////////////
char *doubleAry_to_CharAry( double *double_ary, const int NData){
    char *buf;
	char temp[20];
	int  i;
	
	buf = (char *)malloc((NData+1)*sizeof(double));
	sprintf(temp,"%f",*double_ary);
	printf("%s\n",temp);
	
	//copy the first element into the buffered char array
	strcpy (buf,temp);
	
	// if more than one element in the input array array, iterate over the array; 
	if(NData>1){
	  for(i = 1;i<NData;i++){
	     sprintf(temp,"%f",*(double_ary+i));
	     printf("%s\n",temp);
	     strcat(buf,",");
	     strcat(buf,temp);
	  }
	}
   return buf;
}

//////////////////////////////////////////////////////////////////////////
// convert the comma separated char array (or string) to a double array
// e.g. str ="1.1, 2.2,3.3"
// The resulting double array dbl={1.1,2.2,3.3}
///////////////////////////////////////////////////////////////////////////
void charAry_to_Dbl_Ary (char *str, double *dbl_ary)
{
	
	char *temp=NULL;
	double d;
	int i=0;
	temp=strtok(str," ,"); // both blank and comma as delimiters are supported.
	
	while (temp != NULL)
  {
    printf ("str = %s\n",temp);

	d=strtod(temp,NULL); //use strtod to convert string to double
	printf ("d= %f\n",d);
	*(dbl_ary+i) = d;
	printf("dbl=%f\n", *(dbl_ary+i));
	i++;
    temp = strtok (NULL, " ,");
   }
}

///////////////////////////////////////////////////////////////////////////////
/// @function print_header
/// @description creates a new component logfile with a timestamped header
/// @pre filename must specify a valid filepath for writable files
/// @post filename will be replaced by a new file with a simple header
/// @param filename specifies the filepath of the new component logfile
/// @param address specifies the IPv4 address associated with the component
/// @param port specifies the IP port number associated with the component
///////////////////////////////////////////////////////////////////////////////
void print_header( const char * filename, const char * address, int port )
{
    FILE * logfile;
    time_t rawtime;
    struct tm * timeinfo;
    
    // create new log
    if( (logfile = fopen( filename, "w" )) != 0 )
    {
        // get current time
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        
        // print log header
        fprintf( logfile, "Current Time:   %s", asctime(timeinfo) );
        fprintf( logfile, "Server Address: %s:%d\n", address, port );
        
        fclose(logfile);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @function print_data
/// @description appends data with a simple header to the passed filename
/// @pre filename must specify a valid filepath for writable files
/// @pre length must specify the number of elements in data
/// @post the header and data are appended to the file with filename
/// @param filename specifies the filepath of the file to modify
/// @param header specifies the line of text to preceed the data
/// @param data specifies the data to write. each element will be on a new line
/// @param length specifies the number of data elements which may be 0
///////////////////////////////////////////////////////////////////////////////
void print_data( const char * filename, const char * header,
        const double * data, int length )
{
    FILE * logfile;
    int index;
    
    // append to existing log
    if( (logfile = fopen( filename, "a" )) != 0 )
    {
        // print the simple header
        fprintf( logfile, "%s\n", header );
        
        // print the associated data
        for( index = 0; index < length; index++ )
        {
            fprintf( logfile, "\t%f\n", data[index] );
        }
        fclose(logfile);
    }
}

void save_to_file( const char * filename, char * str)
{
    FILE * logfile;
    
    
    // append to existing log
    if( (logfile = fopen( filename, "a" )) != 0 )
    {
        
        // print the str
        fprintf( logfile, "\t%s\n", str );
        
        fclose(logfile);
    }
}
///////////////////////////////////////////////////////////////////////////////
/// @function print_error
/// @description appends a simple error message to the passed filename
/// @pre filename must specify a valid filepath for writable files
/// @post the error message is appended to the file with filename
/// @param filename specifies the filepath of the file to modify
/// @param error specifies the error message to write to the file
///////////////////////////////////////////////////////////////////////////////
void print_error( const char * filename, const char * error )
{
    // outsource implementation
    print_data( filename, error, 0, 0 );
}

///////////////////////////////////////////////////////////////////////////////
/// @function print_footer
/// @description appends a simple footer to the passed component logfile
/// @pre filename must specify a valid filepath for writable files
/// @post the footer 'Simulation Complete' will be appended to the file
/// @param filename specifies the filepath of the file to modify
///////////////////////////////////////////////////////////////////////////////
void print_footer( const char * filename )
{
    // outsource implementation
    print_data( filename, "Simulation Complete", 0, 0 );
}

///////////////////////////////////////////////////////////////////////////////
/// @function itodd
/// @description converts four integer octets to dot-decimal notation
/// @pre ip1 through ip4 must be a maximum of three digits long
/// @pre address must be able to store at least 16 elements
/// @post address will be modified to contain the dot-decimal notation
/// @param ip1 first octet of the IP address
/// @param ip2 second octet of the IP address
/// @param ip3 third octet of the IP address
/// @param ip4 fourth octet of the IP address
/// @param address c-string to store the dot-decimal result
/// @return if success, new size of address. if failure, an integer < 1
///////////////////////////////////////////////////////////////////////////////
int itodd( int ip1, int ip2, int ip3, int ip4, char * address )
{
    return sprintf( address, "%d.%d.%d.%d", ip1, ip2, ip3, ip4 ) + 1;
}

///////////////////////////////////////////////////////////////////////////////
/// @function connect_to_server
/// @description creates and connects a client socket to a remote server
/// @pre address must specify a valid IPv4 address
/// @pre error must be able to store a standard error message
/// @post new socket created with a connection to the remote server
/// @post error will be modified to store an error message if the call fails
/// @param address specifies the IPv4 address of the remote server
/// @param port specifies the port number of the remote server
/// @param error will store an error message on failure
/// @return if success, the socket connection. if failure, -1.
///////////////////////////////////////////////////////////////////////////////
int connect_to_server( const char * address, int port, char * error )
{
    struct hostent * hostname;
    struct sockaddr_in server;
    int client;
    
    // resolve server hostname
    if( (hostname = gethostbyname(address)) == 0 )
    {
        sprintf(error,"failed to resolve hostname for %s",address);
        return -1;
    }
    
    // create client IPv4 TCP socket
    if( (client = socket( AF_INET, SOCK_STREAM, 0 )) == -1 )
    {
        strcpy(error,strerror(errno));
        return -1;
    }
    
    // specify server details
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    memcpy( &server.sin_addr, hostname->h_addr_list[0], hostname->h_length );
    
    // connect client to server
    if( connect( client, (struct sockaddr *)&server, sizeof(server) ) == -1 )
    {
        strcpy(error,strerror(errno));
        close(client);
        return -1;
    }
    
    return client;
}

///////////////////////////////////////////////////////////////////////////////
/// @function send_packet
/// @description sends a packet of data across an open socket
/// @pre socket must be an TCP open socket connection
/// @pre bytes must specify the size in bytes of the data
/// @post a blocking send call is made on the given socket
/// @post error will be modified to store an error message if the call fails
/// @param socket the file descriptor of an open TCP socket
/// @param header the header to prepend to the packet
/// @param data the payload to attach to the packet
/// @param bytes the size in bytes of the payload
/// @param error will store an error message on failure
/// @return if success, the number of bytes sent. if failure, -1.
///////////////////////////////////////////////////////////////////////////////
int send_packet( int socket, const char * header, const void * data,
        const int NData, char * error )
{
    char * packet;
    int pkt_size;
    int hdr_size;
    int dat_size;
    
    // get size of header
    hdr_size = strlen(header);
    if( hdr_size > PKT_HDR_SIZE-1 )
    {
        sprintf( error, "packet header (%s) exceeds maximum header size (%d)",
                header, PKT_HDR_SIZE-1);
        return -1;
    }
    
    // allocate memory for packet
    //pkt_size = PKT_HDR_SIZE + bytes;
   // packet = (char *)malloc(pkt_size);
    {
        // store header at packet front
        //memcpy( packet, header, hdr_size );
        //packet[hdr_size] = '\0';

        if( NData > 0 )
        {
            // store data after packet header
           // memcpy( packet+PKT_HDR_SIZE, data, bytes );
		   packet=doubleAry_to_CharAry( data, NData);
        }
        
        // send packet across socket
        if( (dat_size = send( socket, packet, (NData+1)*sizeof(double), 0 )) == -1 )
        {
            strcpy(error,strerror(errno));
        }
		else
		  save_to_file( SENDLOG, packet);
    }
    free(packet);
    
    return dat_size;
}

///////////////////////////////////////////////////////////////////////////////
/// @function receive_packet
/// @description receives a packet of data from an open socket
/// @pre socket must be an open TCP socket connection
/// @pre bytes must specify the size in bytes of data
/// @post a blocking receive call is made on the given socket
/// @post error will be modified to store an error message if the call fails
/// @param socket the file descriptor of an open TCP socket
/// @param data the array to store the data received
/// @param bytes the size in bytes of the data array
/// @param error will store an error message on failure
/// @return if success, the number of bytes received. if failure, -1.
///////////////////////////////////////////////////////////////////////////////
int receive_packet( int socket, void * data, int bytes, char * error )
{
    int dat_size;

    // receive packet from socket
    if( (dat_size = recv( socket, data, bytes, 0 )) == -1 )
    {
        strcpy(error,strerror(errno));
    }
    
    return dat_size;
}

///////////////////////////////////////////////////////////////////////////////
/// @function pscad_send_init
/// @description initialization step for the pscad_send component
/// @pre ip1.ip2.ip3.ip4:port must specify a valid IPv4 address
/// @pre length must specify the number of elements in data
/// @post creates the file SENDLOG with a simple header
/// @post sends a RST message to the given IP address
/// @param ip1 first octet of the IP address
/// @param ip2 second octet of the IP address
/// @param ip3 third octet of the IP address
/// @param ip4 fourth octet of the IP address
/// @param port port number of the IP address
/// @param data payload for the RST packet
/// @param length number of elements in the payload
/// @limitations the number of trailing underscores for this function depends
/// on both the version of PSCAD and the Fortran compiler. if this code does
/// not compile, rename the function pscad_send_init_ with a single underscore
///////////////////////////////////////////////////////////////////////////////
void pscad_send_init_( int * ip1, int * ip2, int * ip3, int * ip4, int * port,
        double * data, int * length )
{
    char request[] = "RST";
    char address[16];
    char err[128];
    int sd;

    // get printable ip address
    itodd( *ip1, *ip2, *ip3, *ip4, address );
    print_header( SENDLOG, address, *port );
	//add to create a new recv record file
	print_header( RECVLOG, address, *port );

    // connect to remote simulation server
    if( (sd = connect_to_server( address, *port, err )) == -1 )
    {
        print_error( SENDLOG, err );
        return;
    }

    // send the RST request and payload data
	// Original 
	//if( send_packet( sd, request, data, (*length)*sizeof(double), err ) == -1 )
	// modified [06/11/2013]
    //if( send_packet( sd, request, data, (*length), err ) == -1 )
    // send ONLY REQUEST(Client side READY SIGNAL) across socket
    if(  send( sd, request, strlen(request), 0 ) == -1 )
    {
        strcpy(err,strerror(errno));
        print_error( SENDLOG, err );
        return;
    }
    
    //print_data( SENDLOG, request, data, *length );
	save_to_file( SENDLOG, request);
}

///////////////////////////////////////////////////////////////////////////////
/// @function pscad_send
/// @description send state step for the pscad_send component
/// @pre ip1.ip2.ip3.ip4:port must specify a valid IPv4 address
/// @pre length must specify the number of elements in data
/// @post appends to the file SENDLOG a status message
/// @post sends a SET message to the given IP address
/// @param ip1 first octet of the IP address
/// @param ip2 second octet of the IP address
/// @param ip3 third octet of the IP address
/// @param ip4 fourth octet of the IP address
/// @param port port number of the IP address
/// @param data payload for the SET packet
/// @param length number of elements in the payload
/// @limitations the number of trailing underscores for this function depends
/// on both the version of PSCAD and the Fortran compiler. if this code does
/// not compile, rename the function pscad_send_ with a single underscore
///////////////////////////////////////////////////////////////////////////////
void pscad_send_( int * ip1, int * ip2, int * ip3, int * ip4, int * port,
        double * data, int * length )
{
    char request[] = "SET";
    char address[16];
    char err[128];
    int sd;
    
    // get printable ip address
    itodd( *ip1, *ip2, *ip3, *ip4, address );
    
    // connect to remote simulation server
    if( (sd = connect_to_server( address, *port, err )) == -1 )
    {
        print_error( SENDLOG, err );
        return;
    }
    
    // send the SET request and corresponding data
	//if( send_packet( sd, request, data, (*length)*sizeof(double), err ) == -1 )
    if( send_packet( sd, request, data, (*length), err ) == -1 )
    {
        print_error( SENDLOG, err );
        return;
    }
        
    //print_data( SENDLOG, request, data, *length );
}


///////////////////////////////////////////////////////////////////////////////
/// @function pscad_send_close
/// @description final step for the pscad_send component
/// @pre none
/// @post appends to the file SENDLOG a simple footer
/// @limitations the number of trailing underscores for this function depends
/// on both the version of PSCAD and the Fortran compiler. if this code does
/// not compile, rename the function pscad_send_close_ with a single underscore
///////////////////////////////////////////////////////////////////////////////
void pscad_send_close_( )
{
    print_footer( SENDLOG );
}

///////////////////////////////////////////////////////////////////////////////
/// @function pscad_recv_init
/// @description initialization step for the pscad_recv component
/// @pre none
/// @post creates the file RECVLOG with a simple header
/// @param ip1 first octet of an IP address
/// @param ip2 second octet of an IP address
/// @param ip3 third octet of an IP address
/// @param ip4 fourth octet of an IP address
/// @param port port number of an IP address
/// @limitations the number of trailing underscores for this function depends
/// on both the version of PSCAD and the Fortran compiler. if this code does
/// not compile, rename the function pscad_recv_init_ with a single underscore
///////////////////////////////////////////////////////////////////////////////
void pscad_recv_init_( int * ip1, int * ip2, int * ip3, int * ip4, int * port )
{
    char address[16];
    
    // get printable ip address
    itodd( *ip1, *ip2, *ip3, *ip4, address );
    print_header( RECVLOG, address, *port );
}

///////////////////////////////////////////////////////////////////////////////
/// @function pscad_recv
/// @description receive command step for the pscad_recv component
/// @pre ip1.ip2.ip3.ip4:port must specify a valid IPv4 address
/// @pre length must specify the number of elements in data
/// @post appends to the file RECVLOG a status message
/// @post stores in data the payload received from the given IP address
/// @param ip1 first octet of the IP address
/// @param ip2 second octet of the IP address
/// @param ip3 third octet of the IP address
/// @param ip4 fourth octet of the IP address
/// @param port port number of the IP address
/// @param data array to store the received information
/// @param length number of element of the data array
/// @limitations the number of trailing underscores for this function depends
/// on both the version of PSCAD and the Fortran compiler. if this code does
/// not compile, rename the function pscad_recv_ with a single underscore
///////////////////////////////////////////////////////////////////////////////
void pscad_recv_( int * ip1, int * ip2, int * ip3, int * ip4, int * port,
        double * data, int * length )
{
    char request[] = "GET";
    char address[16];
    char error[128];
	char *tempDataInChar = NULL;
    int sd;
	int char_size =(*length)*(sizeof(double)+1);
    
    // get printable ip address
    itodd( *ip1, *ip2, *ip3, *ip4, address );
    
    // connect to remote simulation server
    if( (sd = connect_to_server( address, *port, error )) == -1 )
    {
        print_error( RECVLOG, error );
        return;
    }
    
    // send the GET request
/*     if( send_packet( sd, request, 0, 0, error ) == -1 )
    {
        print_error( RECVLOG, error );
        return;
    } */
    
	//aLLocate memory for the tempData
	tempDataInChar = (char *)malloc(char_size);
	
	
    // receive the data response
    if( receive_packet( sd, tempDataInChar, char_size, error ) == -1 )
    {
        print_error( RECVLOG, error );
        return;
    }
	
    // save the char array back to double array;
	charAry_to_Dbl_Ary (tempDataInChar, data);
	
	// print the logging
    print_data( RECVLOG, request, data, *length );
}

///////////////////////////////////////////////////////////////////////////////
/// @function pscad_recv_close
/// @description final step for the pscad_recv component
/// @pre none
/// @post appends to the file RECVLOG a simple footer
/// @limitations the number of trailing underscores for this function depends
/// on both the version of PSCAD and the Fortran compiler. if this code does
/// not compile, rename the function pscad_recv_close_ with a single underscore
///////////////////////////////////////////////////////////////////////////////
void pscad_recv_close_( )
{
    print_footer( RECVLOG );
}

///////////////////////////////////////////////////////////////////////
/// @function pscad_send
/// @description send state step for the pscad_send component
/// @pre ip1.ip2.ip3.ip4:port must specify a valid IPv4 address
/// @pre length must specify the number of elements in data
/// @post appends to the file SENDLOG and RECVLOG a status message
/// @param ip1 first octet of the IP address
/// @param ip2 second octet of the IP address
/// @param ip3 third octet of the IP address
/// @param ip4 fourth octet of the IP address
/// @param port port number of the IP address
/// @param sendData double array for the SEND packet
/// @param NData_S number of elements in the SEND packet
/// @param recvData double array for the RECV packet
/// @param NData_R number of elements in the RECV packet
/// @limitations the number of trailing underscores for this function depends
/// on both the version of PSCAD and the Fortran compiler. if this code does
/// not compile, rename the function pscad_send_ with a single underscore
///////////////////////////////////////////////////////////////////////////////
void pscad_send_recv_( int * ip1, int * ip2, int * ip3, int * ip4, int * port,
        double * sendData, int * NData_S,  double * recvData, int * NData_R)
{   
    char sendHeader[] = "SENT";
	char recvHeader[] = "RECV";
    char address[16];
    char err[128];
	int sd;
	
	char *tempDataInChar = NULL;
	int char_size =(*NData_R)*(sizeof(double)+1)-1;
    
    //1. get printable ip address
    itodd( *ip1, *ip2, *ip3, *ip4, address );
    
    //2. connect to remote simulation server
    if( (sd = connect_to_server( address, *port, err )) == -1 )
    {
        print_error( SENDLOG, err );
        return;
    }
    
    // 3. send data
	//if( send_packet( sd, request, data, (*length)*sizeof(double), err ) == -1 )
    if( send_packet( sd, sendHeader, sendData, (*NData_S), err ) == -1 )
    {
        print_error( SENDLOG, err );
        return;
    }
	
	//aLLocate memory for the temp char array for recvData
	tempDataInChar = (char *)malloc(char_size);
	
    //4. receive the data response
    if( receive_packet( sd, tempDataInChar, char_size, err ) == -1 )
    {
        print_error( RECVLOG, err);
        return;
    }
	
    // save the char array back to double array;
	charAry_to_Dbl_Ary(tempDataInChar, recvData);
	
	// print the recv logging
    print_data( RECVLOG, recvHeader, recvData, * NData_R );


}

