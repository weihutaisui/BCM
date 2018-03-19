var count=0;
function set_in_progress() {
	var act=document.getElementsByName("action");
	var prompt=document.getElementsByName("prompt_lb");
	prompt[0].innerText="Reconfiguring Wireless is in progress..";
	act[0].value="....wait....";
	if(count%2==0) 
		prompt[0].innerText="Reconfiguring Wireless is in progress..";
	else
		prompt[0].innerText="Reconfiguring Wireless is still progressing ..";

	act[0].disabled=true;
	count++;
}
var status_url = window.location.protocol +'//' + window.location.host +'/wlstatus.cgi';

function wait_for_ready(dst) {
	$.ajax({
		type:'GET',
		url: status_url,
		data:{Req:'wlstatus'},
		async:true,
		success:function(result){
			console.log(result);
			if(result== "1")  {
				document.location.href=dst;
			} else {
				set_in_progress();
				setTimeout(wait_for_ready,500,dst);
			}

		}
	});
}
