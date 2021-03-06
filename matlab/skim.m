classdef skim < handle
    properties ( Access = private )
        serial_port
        packet_buffer
    end
    methods
        function code = set_gain_offset(obj, gain, offset)
            [pb, req_size, resp_size] = calllib('skim','set_gain_offset', obj.packet_buffer, 0, 0 );
            fwrite(obj.serial_port,obj.packet_buffer.Value(1:req_size), 'uint8' );
            [ pb, count] = fread(obj.serial_port,double(resp_size));
            if count == resp_size 
                obj.packet_buffer.Value = pb;
                [ret, pb, code] = calllib('skim','ack', obj.packet_buffer, 0 );
            else 
                code = -1;
            end
            
        end
        function v = version(obj)
            [pb, req_size, resp_size] = calllib('skim','version_request', obj.packet_buffer, 0, 0 );
            fwrite(obj.serial_port,obj.packet_buffer.Value(1:req_size), 'uint8' );
            [ pb, count] = fread(obj.serial_port,double(resp_size));
            if count == resp_size 
                obj.packet_buffer.Value = pb;
                [ret, pb, v] = calllib('skim','version_response', obj.packet_buffer, 0 );
            else 
                v = 0;
            end
        end
        function obj = skim()
            [notfound,warnings] = loadlibrary('..\protocol\win\x64\Debug\skim.dll','..\protocol\win\skim.h');
            obj.packet_buffer = libpointer('uint8Ptr',zeros( calllib('skim','get_maximum_packet_size'), 1) );
        end
        function delete(obj)
            close();
            if libisloaded('skim')
                unloadlibrary 'skim';
            end
        end
        function open_serial( obj, port )
            obj.serial_port = serial( port );
            set(obj.serial_port,'BaudRate',115200,'Parity','none');
            set(obj.serial_port,'Timeout',1);
            fopen(obj.serial_port);
        end
        function close( obj)
            if ~isempty(obj.serial_port)s
                fclose( obj.serial_port );
            end
        end
    end
end
