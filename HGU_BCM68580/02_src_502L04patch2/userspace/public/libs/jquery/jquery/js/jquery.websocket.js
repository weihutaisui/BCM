/*!
* jQuery websocket Library
* version 0.2
* http://www.broadcom.com/
*
* Copyright 2011, Broadcom Corporation
* Dual licensed under the MIT or GPL Version 2 licenses.
* http://jquery.org/license
*
* websocket calls use "id" field to track the caller.
* Server side need to return the id.
* 
* Version history:
* 0.1: 11/1/11. Created. 
*   Requires jQuery 1.6.4.
*
* 0.2: 11/11/11. Added defer to send function.
*   Requires jQuery 1.7
* 
* 0.3: 11/15/11. Added optional onopen, onclose, onerror arguments.
*
* 0.4: 2/8/14. Added keep paramter to send function to keep the dfd around the notify it when msg arrives.
*
* */
(function ($) {
    $.websocket = function (url, rpc, onopen, onclose, onerror, onmessage) {
        var ws = rpc ? new WebSocket(url, rpc) : new WebSocket(url);

        $(ws).bind("message", function(e) {
            if (onmessage) {
                onmessage.call(this, e);
                return;
            }
            var s = $.evalJSON(e.originalEvent.data);
            var dfd = getDeferred.call(this, s);
            if (dfd) {
                if (!(dfd.__keep === null || dfd.__keep === undefined || dfd.__keep == false))
                    dfd.notify(s);
                else
                    dfd.resolve(s);
            }
        }).bind("error", function (e) {
            var d = e.originalEvent.data ? e.originalEvent.data : e.data;
            if (d) {
                var s = $.evalJSON(d);
                var dfd = getDeferred.call(this, s);
                if (dfd)
                    dfd.reject(s);
            }
            else if (!onerror)
                alert(d);
        });

		if (onopen) $(ws).bind("open", onopen);
		if (onclose) $(ws).bind("close", onclose);
		if (onerror) $(ws).bind("error", onerror);
		
        function getDeferred(s) {
            var id = s.id.toString();
            var dfd = $(this).data(id);
            if (dfd && (dfd.__keep === null || dfd.__keep === undefined || dfd.__keep == false))
                $(this).removeData(id);
            return dfd;
        };

        var _send = ws.send;
        /*The send command returns a Deferred object.
        *  It also optionally take a deferred object and wait for it to finish before sending.
        *  See http://api.jquery.com/category/deferred-object/ for usage.
        */
        ws.send = function (method, params, id, keep, defer) {
            if (id === null || id === undefined)
                id = (10000000 * Math.random()).toFixed();
            id = id.toString();
            var m = {
                jsonrpc: "2.0",
                method: method,
                params: params,
                id: id
            };
            var s = $.toJSON(m);
            var dfd = $.extend($(this).data(id), $.Deferred());
            dfd.__keep = keep; // keep the dfd for future callbacks
            $(this).data(id, dfd);
            if (defer === null || defer === undefined)
                _send.call(this, s);
            else
                defer.always(function () {
                    dfd.notify(); // notify our progress
                    _send.call(ws, s);
                });
            return dfd;
        };

        window.onbeforeunload = function() {
            ws.close();
        };
        return ws;
    }
})(jQuery);