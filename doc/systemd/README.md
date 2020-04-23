## UltraNote Systemctl Daemon

Make sure your UltraNote infrastructure keeps running after a reboot or after any failure. For this we recommand systemd

Make sure you have the binaries downloaded in advance.

To use the UltraNote Daemon Service, update the `ultranoted.service` file.
Next, copy the `ultranoted.service` file to `/etc/systemd/system/ultranoted.service`.
After that, copy the `ultranoted` binary to `/usr/bin/ultranoted`.

Now test the setup with `sudo systemctl start ultranoted`.
To check the status, run `sudo systemctl status ultranoted`.
Once it works, run `sudo systemctl enable ultranoted` to make sure ultranoted starts at boot and after a failure.

Restarting: `sudo systemctl restart ultranoted`
Stopping: `sudo systemctl stop ultranoted`
Starting: `sudo systemctl start ultranoted`

*Same process goes for `ultranote-walletd.service`*

### Running as a remote node use

`ExecStart=/usr/bin/ultranoted --data-dir=/home/YOUR_USER/.UltraNote --rpc-bind-ip=0.0.0.0 --rpc-bind-port=33000`
