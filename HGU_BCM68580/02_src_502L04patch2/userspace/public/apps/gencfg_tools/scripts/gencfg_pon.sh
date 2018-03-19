#!/bin/bash

wantype="`pspctl dump RdpaWanType`"

case "$wantype" in
    'GPON') 
        sleep 2
        echo "Configure GPON"
        bs /b/c gpon password=20202020202020202020
        bs /b/c gpon onu_sn={vendor_id=0x4252434d,vendor_specific=0x12345678}
        bs /b/c gpon link_activate=activate_O1
        ;;
        
    'EPON')
        sleep 2
        echo "Configure EPON"
        bs /b/c filter etype_udef[0]=0x888e
        bs /b/c filter etype_udef[1]=0x8809
        bs /b/c filter etype_udef[2]=0x8808
        bs /b/c filter entry[{filter=etype_udef_0,ports=wan0}]={enabled=yes,action=host}
        bs /b/c filter entry[{filter=etype_udef_0,ports=lan0}]={enabled=yes,action=host}
        bs /b/c filter entry[{filter=etype_udef_0,ports=lan1}]={enabled=yes,action=host}
        bs /b/c filter entry[{filter=etype_udef_0,ports=lan2}]={enabled=yes,action=host}
        bs /b/c filter entry[{filter=etype_udef_0,ports=lan3}]={enabled=yes,action=host}
        bs /b/c filter entry[{filter=etype_udef_1,ports=wan0}]={enabled=yes,action=host}
        bs /b/c filter entry[{filter=etype_udef_2,ports=wan0}]={enabled=yes,action=host}
        eponapp &
        ;;
        
    'GBE')              
        echo "Configure GBE"  
        ;;
    *)  
        echo "Undefined RdpaWanType $wantype" 
        ;;
esac          


