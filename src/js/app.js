// https://jslinterrors.com/use-the-function-form-of-use-strict
// 'use strict';
// https://jslinterrors.com/a-is-better-written-in-dot-notation
/* jshint -W069 */
/* global WebSocket */

var WS_URL = 'ws://vibeface.lochotzke.name:8000';
var ws = null;

Pebble.addEventListener('ready', function () {
    console.log('[JS] Ready');

    ws = new WebSocket(WS_URL);
    ws.onopen = function() {
        console.log('[JS] Connected to ' + WS_URL);
    };
    ws.onclose = function(e) {
        console.log('[JS] Disconnected from ' + WS_URL + ' because of ' + e.reason);
        ws = null;
    };
    ws.onmessage = function(e) {
        if (e.data) {
            console.log('[JS] Received message');
            var dict = JSON.parse(e.data);

            if ('vibrate' in dict) {
                Pebble.sendAppMessage({ KEY_VIBRATE: dict['vibrate'] }, function (e) {
                    console.log('[JS] AppMessage ' + e.data.transactionId + ' succeeded: ' + JSON.stringify(dict));
                }, function (e) {
                    console.log('[JS] AppMessage ' + e.data.transactionId + ' failed: ' + JSON.stringify(e));
                });
            }
        } else {
            console.log('[JS] Received message without data');
        }
    };
    ws.onerror = function() {
        console.log('[JS] Error');
    };
});
