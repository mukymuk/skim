%% open
loadlibrary('C:\Users\Shawn\project\skim\trunk\protocol\win\x64\Debug\skim.dll','C:\Users\Shawn\project\skim\trunk\protocol\win\skim.h');
serial_port = serial('COM3');
set(serial_port,'BaudRate',115200,'Parity','none');
tx_packet_buffer = libpointer('uint8Ptr',zeros( calllib('skim','get_maximum_packet_size'), 1) );
rx_packet_buffer = libpointer('uint8Ptr',zeros( 6, 1) );
fopen(serial_port);

%% get version

[pb, req_size, resp_size]=calllib('skim','version_request',tx_packet_buffer, 0, 0 );
fwrite(serial_port,tx_packet_buffer.Value(1:req_size) );
rx_packet_buffer.Value = fread(serial_port,double(resp_size));
[ret, pb, version] = calllib('skim','version_response', rx_packet_buffer, 0 );
clear req_size resp_size p_double;

%% close
fclose( serial_port );
clear serial_port packet_buffer;
unloadlibrary 'skim'

