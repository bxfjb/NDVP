do
    --协议名称为NAP，在Packet Details窗格显示为New Distance Vector Protocol
    local p_NAP = Proto("NAP","New Application Protocol")
    --Header
    local f_type = ProtoField.uint8("NAP.type","type", base.DEC)
    local f_checksum = ProtoField.uint8("NAP.checksum", "checksum", base.HEX)

    --Payload
    local f_sid = ProtoField.uint16("NAP.sid", "sid", base.DEC)
    local f_criteria = ProtoField.uint16("NAP.criteria", "criteria", base.DEC)
    local f_stream_number = ProtoField.uint16("NAP.stream_number", "stream_number", base.DEC)
    local f_k = ProtoField.uint32("NAP.Info.K", "K", base.DEC)
    local f_k1 = ProtoField.uint32("NAP.Info.K1", "K1", base.DEC)
    local f_w = ProtoField.uint32("NAP.Info.W", "W", base.DEC)

    local f_content_length = ProtoField.uint16("NAP.content_length", "content_length", base.DEC)
    local f_label = ProtoField.uint16("NAP.label", "label", base.DEC)
    local f_payload = ProtoField.string("NAP.payload", "payload", base.ASCII)

    --这里把NAP协议的全部字段都加到p_NAP这个变量的fields字段里
    p_NAP.fields = {
        f_type, f_checksum,
        f_sid, f_criteria, f_stream_number, f_k, f_k1, f_w,
        f_content_length,  f_label, f_payload
    }

    local function get_packet_type(type)
        local packet_type = "Unknown Type"

        if type == 1 then 
            packet_type = "NAP_REQUEST"
        elseif type == 2 then 
            packet_type = "NAP_RESPONSE"
        elseif type == 3 then
            packet_type = "NAP_PAYLOAD"
        end

        return packet_type
    end

    local function get_criteria_type(type)
        local criteria_type = "Unknown Type"
        if type == 1 then 
            criteria_type = "Shortest-Widest"
        elseif type == 2 then 
            criteria_type = "Widest-Shortest"
        elseif type == 3 then
            criteria_type = "K-Quickest order"
        elseif type == 4 then
            criteria_type = "W-wide-Shortest order"
        elseif type == 5 then
            criteria_type = "Neo-K-Quickest order"
        elseif type == 6 then
            criteria_type = "Neo-W-wide-Shortest order"
        end

        return criteria_type
    end

    function p_NAP.dissector(buff,pinfo,tree)
        buf_len = buff:len()
        --先检查报文长度
        if buf_len == 0 then return end

        --在Packet List窗格的Protocol列可以展示出协议的名称
        pinfo.cols.protocol = p_NAP.name

        --Tree
        local subtree = tree:add(p_NAP,buff(),"New Application Protocol Data")
        local header_subtree = subtree:add(p_NAP, buff(), "Header")
        local payload_subtree = subtree:add(p_NAP, buff(), "Payload")

        --Header
        local p_type = get_packet_type(buff(0,1):uint())
        header_subtree:add(f_type,buff(0,1)):append_text(" ("..p_type..")")
        header_subtree:add(f_checksum,buff(1,1))

        --Payload
        local v_type = buff(0,1):uint()
        if v_type == 1 then
            local c_type  = get_criteria_type(buff(4,2):uint())
            payload_subtree:add(f_sid,buff(2,2))
            payload_subtree:add(f_criteria,buff(4,2)):append_text(" ("..c_type..")")
            payload_subtree:add(f_stream_number,buff(6,2))
            local v_cri = buff(4,2):uint()
            if v_cri == 3 then
                payload_subtree:add(f_k,buff(8,4))
            elseif v_cri == 4 then
                payload_subtree:add(f_w,buff(8,4))
            elseif v_cri == 5 then
                payload_subtree:add(f_k,buff(8,4))
                payload_subtree:add(f_k1,buff(12,4))
            elseif v_cri == 6 then
                payload_subtree:add(f_k,buff(8,4))
                payload_subtree:add(f_k1,buff(12,4))
                payload_subtree:add(f_w,buff(16,4))
            end
        elseif v_type == 2 then
            payload_subtree:add(f_stream_number,buff(2,2))
        elseif v_type == 3 then
            local v_content_len = buff(2,2):uint()
            payload_subtree:add(f_content_length,buff(2,2)):append_text("*16Byte")
            payload_subtree:add(f_label,buff(4,2))
            payload_subtree:add(f_stream_number,buff(6,2))
            payload_subtree:add(f_payload,buff(8,v_content_len*16))
        end
        return
    end

    local udp_port = DissectorTable.get("udp.port")
    --因为我们的NAP协议的接受端口肯定是12346，所以这里只需要添加到"udp.port"这个DissectorTable里，并且指定值为12346即可。
    udp_port:add(12346, p_NAP)
end
