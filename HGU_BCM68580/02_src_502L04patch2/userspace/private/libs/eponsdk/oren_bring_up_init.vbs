#$language = "VBScript"
#$interface = "1.0"

Sub SendLinuxCommand(shell_command)
   crt.Screen.Send shell_command & VbCr
   crt.screen.WaitForString "#"
   crt.Sleep 200
End Sub

Sub SendLinuxCommandAndVerify(shell_command)
   crt.Screen.Send shell_command & VbCr
   crt.screen.WaitForString "success"
   crt.Sleep 1000
End Sub

Sub SendShellCommand(shell_command)
   crt.Screen.Send shell_command & VbCr
   crt.screen.WaitForString ">"
   crt.Sleep 200
End Sub


Sub SendLogin()
   crt.Screen.Send "admin" & VbCr
   crt.sleep 200
   crt.Screen.Send "admin" & VbCr
   crt.sleep 200
End Sub


Sub main

	SendLogin ( )

	SendLinuxCommand ( "sh" )
	SendLinuxCommand ( "killall eponapp" )
	SendLinuxCommand ( "ifconfig eth0 down" )
	SendLinuxCommand ( "/sbin/insmod /lib/modules/3.4.11-rt19/extra/bdmf.ko bdmf_chrdev_major=215 bdmf_global_trace_level=2" )
	SendLinuxCommand ( "/sbin/insmod /lib/modules/3.4.11-rt19/extra/gpon_stack.ko" )
	SendLinuxCommand ( "/sbin/insmod /lib/modules/3.4.11-rt19/extra/rdpa_gpl.ko" )
	SendLinuxCommand ( "/sbin/insmod /lib/modules/3.4.11-rt19/extra/rdpa.ko" )

	' Initialize bdmf shell
	SendLinuxCommand ( "bdmf_shell -c init | while read a b ; do echo $b ; done > /var/bdmf_sh_id" )
	SendLinuxCommand ( "alias bs=""bdmf_shell -c `cat /var/bdmf_sh_id` -cmd """ )

   	' Create system object
    	SendShellCommand ( "bs /bdmf/new system/init_cfg={wan_type=epon,emac_mode={group_mode=qsgmii,emac4_mode=rgmii,emac5_mode=sgmii},num_lan=4,headroom_size=0,enabled_emac=emac0+emac1+emac2+emac3,switching_mode=none,ip_class_method=fc}" )

        'set emac tpo port
	SendLinuxCommandAndVerify ( "bs /bdmf/new port/index=lan0,cfg={emac=emac0},emac_cfg={enable=true}" )
	SendLinuxCommandAndVerify ( "bs /bdmf/new port/index=lan1,cfg={emac=emac1},emac_cfg={enable=true}" )
	SendLinuxCommandAndVerify ( "bs /bdmf/new port/index=lan2,cfg={emac=emac2},emac_cfg={enable=true}" )
	SendLinuxCommandAndVerify ( "bs /bdmf/new port/index=lan3,cfg={emac=emac3},emac_cfg={enable=true}" )
	
	'set DS egress TM 
	SendLinuxCommandAndVerify ( "bs /bdmf/new egress_tm/dir=ds,index=0,level=queue,mode=sp port/index=lan0")
	SendLinuxCommandAndVerify ( "bs /bdmf/new egress_tm/dir=ds,index=1,level=queue,mode=sp port/index=lan1")
	SendLinuxCommandAndVerify ( "bs /bdmf/new egress_tm/dir=ds,index=2,level=queue,mode=sp port/index=lan2")
	SendLinuxCommandAndVerify ( "bs /bdmf/new egress_tm/dir=ds,index=3,level=queue,mode=sp port/index=lan3")
	

	'Configure DS priority queues for LAN emac ports (emac ports schedulers have been already configured)
	SendLinuxCommandAndVerify ( "bs /bdmf/configure egress_tm/dir=ds,index=0 queue_cfg[0]={queue_id=0,weight=0,drop_alg=dt,drop_threshold=128,red_threshold=0,red_percent=0}" )
	SendLinuxCommandAndVerify ( "bs /bdmf/configure egress_tm/dir=ds,index=1 queue_cfg[0]={queue_id=0,weight=0,drop_alg=dt,drop_threshold=128,red_threshold=0,red_percent=0}" )
	SendLinuxCommandAndVerify ( "bs /bdmf/configure egress_tm/dir=ds,index=2 queue_cfg[0]={queue_id=0,weight=0,drop_alg=dt,drop_threshold=128,red_threshold=0,red_percent=0}" )
	SendLinuxCommandAndVerify ( "bs /bdmf/configure egress_tm/dir=ds,index=3 queue_cfg[0]={queue_id=0,weight=0,drop_alg=dt,drop_threshold=128,red_threshold=0,red_percent=0}" )

	' Configure CPU queue 0 for debugging purposes 
	' Giving 0 (=NULL) for callback will let us to read packets from the queueu manually from shell.
	SendLinuxCommandAndVerify ( "bs /bdmf/configure cpu/index=host rxq_cfg[0]=""{rx_isr=0,sysb_type=skb,size=128,dump_data=no,isr_priv=0}""" )

	'mandatory at the moment. must have a gpon object as the tcont parent
	SendLinuxCommandAndVerify ( "bs /bdmf/new gpon") 
	SendLinuxCommandAndVerify ( "bs /bdmf/new port/index=wan0" )

	'in epon mode, the alloc is not needed
	'alloc_id is not relevant in epon
	SendLinuxCommandAndVerify ( "bs /bdmf/new tcont/index=0" )

	'create gem, gem port value is irrelevant
	SendLinuxCommandAndVerify ( "bs /bdmf/new gem/index=0,flow_type=ethernet,gem_port=0,us_cfg={tcont={tcont/index=0}}" )
	SendLinuxCommandAndVerify ( "bs /bdmf/configure gem/index=0 ds_cfg={discard_prty=low,destination=eth}" ) 

   'scheduler to tcont
	SendLinuxCommandAndVerify ( "bs /bdmf/new egress_tm/dir=us,index=0,level=queue,mode=sp tcont/index=0" )
   'may need to change index below!!!!!!!!!
	SendLinuxCommandAndVerify ( "bs /bdmf/configure egress_tm/dir=us,index=0 queue_cfg[0]=""{queue_id=0,weight=0,drop_alg=dt,drop_threshold=128,red_threshold=0,red_percent=0}""" )
	
	'create tcont for data
	'alloc_id is not relevant in epon
	SendLinuxCommandAndVerify ( "bs /bdmf/new tcont/index=1" )

	'create gem with port 0
	SendLinuxCommandAndVerify ( "bs /bdmf/new gem/index=1,flow_type=ethernet,gem_port=0,us_cfg={tcont={tcont/index=1}}" )
	SendLinuxCommandAndVerify ( "bs /bdmf/configure gem/index=1 ds_cfg={discard_prty=low,destination=eth}" ) 
	
   'scheduler to tcont
	SendLinuxCommandAndVerify ( "bs /bdmf/new egress_tm/dir=us,index=1,level=queue,mode=sp tcont/index=1" )
   'may need to change index below!!!!!!!!!
	SendLinuxCommandAndVerify ( "bs /bdmf/configure egress_tm/dir=us,index=1 queue_cfg[0]=""{queue_id=0,weight=0,drop_alg=dt,drop_threshold=128,red_threshold=0,red_percent=0}""" )
	SendLinuxCommandAndVerify ( "bs /bdmf/configure egress_tm/dir=us,index=1 queue_cfg[1]=""{queue_id=1,weight=0,drop_alg=dt,drop_threshold=128,red_threshold=0,red_percent=0}""" )
	SendLinuxCommandAndVerify ( "bs /bdmf/configure egress_tm/dir=us,index=1 queue_cfg[2]=""{queue_id=2,weight=0,drop_alg=dt,drop_threshold=128,red_threshold=0,red_percent=0}""" )
	SendLinuxCommandAndVerify ( "bs /bdmf/configure egress_tm/dir=us,index=1 queue_cfg[3]=""{queue_id=3,weight=0,drop_alg=dt,drop_threshold=128,red_threshold=0,red_percent=0}""" )
	SendLinuxCommandAndVerify ( "bs /bdmf/configure egress_tm/dir=us,index=1 queue_cfg[4]=""{queue_id=4,weight=0,drop_alg=dt,drop_threshold=128,red_threshold=0,red_percent=0}""" )
	SendLinuxCommandAndVerify ( "bs /bdmf/configure egress_tm/dir=us,index=1 queue_cfg[5]=""{queue_id=5,weight=0,drop_alg=dt,drop_threshold=128,red_threshold=0,red_percent=0}""" )
	SendLinuxCommandAndVerify ( "bs /bdmf/configure egress_tm/dir=us,index=1 queue_cfg[6]=""{queue_id=6,weight=0,drop_alg=dt,drop_threshold=128,red_threshold=0,red_percent=0}""" )
	SendLinuxCommandAndVerify ( "bs /bdmf/configure egress_tm/dir=us,index=1 queue_cfg[7]=""{queue_id=7,weight=0,drop_alg=dt,drop_threshold=128,red_threshold=0,red_percent=0}""" )
	
	' Configure system bridge
	SendLinuxCommandAndVerify ( "bs /bdmf/new bridge" )
	SendLinuxCommandAndVerify ( "bs /bdmf/link port/index=lan0 bridge/index=0" )
	SendLinuxCommandAndVerify ( "bs /bdmf/link port/index=lan1 bridge/index=0" )
	SendLinuxCommandAndVerify ( "bs /bdmf/link port/index=lan2 bridge/index=0" )
	SendLinuxCommandAndVerify ( "bs /bdmf/link port/index=lan3 bridge/index=0" )

   SendLinuxCommandAndVerify ( "bs /bdmf/configure bridge/index=0 mac['<mac=00:00:00:00:00:a1>']='<ports=lan2,sal_action=forward,dal_action=forward>'")
   SendLinuxCommandAndVerify ( "bs /bdmf/configure bridge/index=0 mac['<mac=00:00:00:00:00:b1>']='<ports=wan0,sal_action=forward,dal_action=forward>'")
   
   SendLinuxCommandAndVerify ( "bs /b/n filter/global_cfg={ls_enabled=yes}" ) 

   'iptv: create object and gem flow 16. multicast packet always arrive from EPON MAC on flow 16. 
   SendLinuxCommandAndVerify ("bs /bdmf/new iptv/lookup_method=mac")
   SendLinuxCommandAndVerify ("bs /bdmf/new gem/index=16,flow_type=ethernet,gem_port=0,ds_cfg={destination=iptv}")
   'configure a L2 multicast table entry
   SendLinuxCommandAndVerify ("bs /bdmf/attr/add iptv channel_request string channel_key={gr_mac=00:EC:AF:1A:90:52},mcast_result={egress_port=lan2}")
   
   'flow cache
   'us
   'wan_flow==gem-id
   SendLinuxCommandAndVerify ( "bs /bdmf/attr/add ip_class flow string key={dir=us,src_ip=192.168.1.100,dst_ip=2.0.0.0,prot=17,src_port=1000,dst_port=2000},result={action=forward,trap_reason=conn_trap0,nat_port=1001,nat_ip=192.168.88.10,port=wan0,wan_flow=1,queue_id=0,ovid_offset=offset_12,l2_head_size=18,action_vec=nat+ttl,l2_header=0000000000b10000000000a1810001000800}")
   'ds
   SendLinuxCommandAndVerify ( "bs /bdmf/attr/add ip_class flow string key={dir=ds,src_ip=2.0.0.0,dst_ip=192.168.1.100,prot=17,src_port=2000,dst_port=1000},result={action=forward,trap_reason=conn_trap0,nat_port=1001,nat_ip=192.168.88.10,port=lan2,queue_id=0,ovid_offset=offset_12,l2_head_size=18,action_vec=nat+ttl,l2_header=0000000000a10000000000b1810001000800}")
   
   'bs /bdmf/conf cpu/index=host read_packet[0]=1
   'bs /bdmf/c gpon tcont_counter_assigment=1
   
   ' SendLinuxCommandAndVerify ( "bs /b/c cpu/index=host rxq_cfg[3]={dump_data=yes}" )
   ' bs /driver/rdd ?
   ' bs /b/c cpu/index=host tx_dump={enable=true}
   ' bs /b/c cpu/index=host tx_dump={enable=false}
   ' bs /driver/rdd pfc 0 3
   ' bs /driver/rdd pvdc
   ' bs /driver/rdd pbpc
   ' bs /b/c cpu/index=host rxq_cfg[3]={dump_data=no} 
   'flow cache
   'us
   'wan_flow==gem-id
   'SendLinuxCommandAndVerify ( "bs /bdmf/attr/add ip_class flow string key={dir=us,src_ip=192.168.1.100,dst_ip=2.0.0.0,prot=17,src_port=1000,dst_port=2000},result={action=host,trap_reason=conn_trap0,nat_port=1001,nat_ip=192.168.88.10,prty={group=0,priority=0},port=wan0,wan_flow=2,ovid_offset=offset_12,l2_head_size=18,action_vec=nat+ttl,l2_header=0000000000b10000000000a1810001000800}")
   'ds
   'SendLinuxCommandAndVerify ( "bs /bdmf/attr/add ip_class flow string key={dir=ds,src_ip=2.0.0.0,dst_ip=192.168.1.100,prot=17,src_port=2000,dst_port=1000},result={action=host,trap_reason=conn_trap0,nat_port=1001,nat_ip=192.168.88.10,prty={group=0,priority=0},port=lan2,wan_flow=2,ovid_offset=offset_12,l2_head_size=18,action_vec=nat+ttl,l2_header=0000000000a10000000000b1810001000800}")
   

   'SendLinuxCommandAndVerify ( "bs /b/c cpu/index=host rxq_cfg[3]={dump_data=yes}" )
   SendLinuxCommandAndVerify ( "bs /b/trace debug" )
   'SendLinuxCommandAndVerify ( "bs /b/c cpu/index=host rxq_cfg[6]={dump_data=no}")
   'SendLinuxCommandAndVerify ( "bs /b/c cpu/index=host tx_dump={enable=true}" )
   
   ' 3723/stats/>lifd 0
   ' bs /b/c cpu/index=host send_packet[{method=port,port=wan0,queue_id=0,wan_flow=0,as=skb}]=080045679999999999
   
   SendLinuxCommand ( "/sbin/insmod /lib/modules/3.4.11-rt19/extra/bcm_enet.ko" )
   SendLinuxCommand ( "/sbin/insmod /lib/modules/3.4.11-rt19/extra/epon_stack.ko epon_usr_init=1" )
   SendLinuxCommand ( "eponctl gather 0" )
   SendLinuxCommand ( "eponapp  -v 0 -m 0 &" )
   SendLinuxCommand ( "ifconfig epon0 192.168.2.1" )
   'SendLinuxCommand ( "ifconfig epon0 up" )
   
end sub
