# Arduino-web-actuators
A simple arduino web server providing a rest based connection to control two linear actuators for the wemos D1 wifi enabled board.

### Usage.
1. Change relayPairMonikers to the application you want
2. Change the ssid and password, compile and deploy.
3. Note the ip in the serial monitor.
4. Open browser and navigate to the -- `http://[ip you noted]</b>/` i.e. `http://192.168.0.37/`

##### Rest structure
 *http://[ipaddress]/[relaypairname]/[anyActionCommand]*
 i.e. `http://192.168.0.1/relaypair1/on`


##### Here's how I'm using it.
I have two linear actuators wired to the relays, switching the polarity and, therefore, extending and retracting the linear actuators.
Relay pair 1 extends a bar and relay pair 2 lifts and lowers a television.

My relay pairs are named as follows;
`const String relayPairMonikers[2] = {"bar", "tv"};`

My Actions are as follows;
`const String ActionCommandsOn[] = {"open", "on", "extend","lift"};`
`const String ActionCommandsOff[] = {"close", "off", "retract","lower"};`

##### To use, I navigate to;

| url | action |
|---|---|
| http://192.168.0.37/bar/open|Extend Actuator 1 (open bar)|
| http://192.168.0.37/bar/close|Retract Actuator 1 (close bar)|
| http://192.168.0.37/tv/lower|Extend Actuator 2 (lower tv)|
| http://192.168.0.37/tv/lift|Retract Actuator 2 (lift tv)|
