do
    --协议名称为NDVP，在Packet Details窗格显示为New Distance Vector Protocol
    local p_NDVP = Proto("NDVP","New Distance Vector Protocol")
    --Header
    local f_type = ProtoField.uint8("NDVP.type","type", base.DEC)
    local f_checksum = ProtoField.uint8("NDVP.checksum", "checksum", base.HEX)

    --Payload
    local f_router_id = ProtoField.uint16("NDVP.originator_id", "originator_id", base.DEC)
    local f_as_number = ProtoField.uint32("NDVP.as_number", "as_number", base.DEC)

    local f_paths_number = ProtoField.uint32("NDVP.path_number", "path_number", base.DEC)
    local f_path_sid = ProtoField.uint16("NDVP.path.sid", "sid", base.DEC)
    local f_path_label = ProtoField.uint16("NDVP.path.label", "label", base.DEC)
    local f_path_delay = ProtoField.uint32("NDVP.path.delay", "delay", base.DEC)
    local f_path_bandwidth = ProtoField.uint32("NDVP.path.bandwidth", "bandwidth", base.DEC)
    local f_path_com = ProtoField.uint32("NDVP.path.computing_rate", "computing_rate", base.DEC)

    local f_paths_invalid_number = ProtoField.uint32("NDVP.invalid_path_number", "invalid_path_number", base.DEC)
    local f_path_invalid_sid = ProtoField.uint16("NDVP.invalid_path.sid", "sid", base.DEC)
    local f_path_invalid_label = ProtoField.uint16("NDVP.invalid_path.label", "label", base.DEC)

    local f_sid = ProtoField.uint16("NDVP.com_adver.sid", "sid", base.DEC)
    local f_com_rate = ProtoField.uint32("NDVP.com_adver.com_rate", "rate", base.DEC)

    --这里把NDVP协议的全部字段都加到p_NDVP这个变量的fields字段里
    p_NDVP.fields = {
        f_type, f_checksum, f_router_id,
        f_as_number,
        f_paths_number, f_path_sid, f_path_label, f_path_delay, f_path_bandwidth, f_path_com,
        f_paths_invalid_number,  f_path_invalid_sid, f_path_invalid_label,
        f_sid, f_com_rate
    }

    local function get_packet_type(type)
        local packet_type = "Unknown Type"

        if type == 1 then 
            packet_type = "NDVP_HELLO"
        elseif type == 2 then 
            packet_type = "NDVP_ADVERTISE"
        elseif type == 3 then
            packet_type = "NDVP_INVALID"
        elseif typ3 == 4 then
            packet_type = "NDVP_COMPUTATION_ADVERTISE"
        end

        return packet_type
    end

    function p_NDVP.dissector(buff,pinfo,tree)
        buf_len = buff:len()
        --先检查报文长度
        if buf_len == 0 then return end

        --在Packet List窗格的Protocol列可以展示出协议的名称
        pinfo.cols.protocol = p_NDVP.name

        --Tree
        local subtree = tree:add(p_NDVP,buff(),"New Distance Vector Protocol Data")
        local header_subtree = subtree:add(p_NDVP, buff(), "Header")
        local payload_subtree = subtree:add(p_NDVP, buff(), "Payload")

        --Header
        local p_type = get_packet_type(buff(0,1):uint())
        header_subtree:add(f_type,buff(0,1)):append_text(" ("..p_type..")")
        header_subtree:add(f_checksum,buff(1,1))
        

        --Payload
        local v_type = buff(0,1):uint()
        if v_type == 1 then
            payload_subtree:add(f_router_id, buff(2,2))
            payload_subtree:add(f_as_number,buff(4,4))
        elseif v_type == 2 then
            payload_subtree:add(f_router_id, buff(2,2))
            local paths_subtree = payload_subtree:add(p_NDVP, buff(), "Paths")
            local v_path_number = buff(4,4):uint()
            payload_subtree:add(f_paths_number,buff(4,4))
            for i = 0, v_path_number-1 do
                local path_name = string.format("path[%d] information",i)
                local sing_path_subtree = paths_subtree:add(p_NDVP, buff(), path_name)

                sing_path_subtree:add(f_path_sid, buff(16*i+8, 2))
                sing_path_subtree:add(f_path_label, buff(16*i+10, 2))
                sing_path_subtree:add(f_path_delay, buff(16*i+12, 4))
                sing_path_subtree:add(f_path_bandwidth, buff(16*i+16, 4))
                sing_path_subtree:add(f_path_com, buff(16*i+20, 4))
            end
        elseif v_type == 3 then
            payload_subtree:add(f_router_id, buff(2,2))
            local paths_invalid_subtree = payload_subtree:add(p_NDVP, buff(), "Invalid Paths")
            local v_path_invalid_number = buff(4,4):uint()
            payload_subtree:add(f_paths_invalid_number,buff(4,4))
            for i = 0, v_path_invalid_number-1 do
                local path_invalid_name = string.format("Invalid path[%d] information",i)
                local sing_path_invalid_subtree = paths_invalid_subtree:add(p_NDVP, buff(), path_invalid_name)

                sing_path_invalid_subtree:add(f_path_invalid_sid, buff(4*i+8, 2))
                sing_path_invalid_subtree:add(f_path_invalid_label, buff(4*i+10, 2))
            end
        elseif v_type == 4 then
            payload_subtree:add(f_sid, buff(2,2))
            payload_subtree:add(f_com_rate,buff(4,4))
        end
        return
    end

    local udp_port = DissectorTable.get("udp.port")
    --因为我们的NDVP协议的接受端口肯定是12345，所以这里只需要添加到"udp.port"这个DissectorTable里，并且指定值为12345即可。
    udp_port:add(12345, p_NDVP)
end
