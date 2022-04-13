-------
<p dir="auto" align="center"><img align="center" src="https://cdn.discordapp.com/attachments/955207154246246410/963828611922677790/unknown.png"></p>

-------
## Infomation

According to Ankit Anubhav, a half of Lantronix device servers are exposed online  leaking their Telnet passwords. An attacker can take over the device via Telnet and use the privileged access to send serial commands to the connected devices.

You can read a full blog post about the Telnet leak in this [post](https://securityaffairs.co/wordpress/66275/hacking/lantronix-serial-to-ethernet-leakage.html)

## Targets

### Tested Devices
- Lantronix X5P 6.9.0.3
- LantronixWVRC4+
- Lantronix X9D 2.3.0.0 
- Lantronix X90 6.9.0.2 

### Shodan Query

[Lantronix password port:30718 -secured](https://www.shodan.io/search?query=Lantronix+password+port%3A30718+-secured)

## Basic Usage

### Compiling

```bash
gcc main.c -o leakyleaky
```

### Example
```bash
./leakyleaky 144.118.206.229 --port=30718
```
