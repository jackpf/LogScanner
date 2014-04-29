LogScanner
==========

This is a simple service for linux to monitor Apache log files in real time and send a pushover notification when a new PHP error is detected.

In order to make you must have glib, pcre and yaml libraries installed.

To install and set to run on startup:

```shell
git clone https://github.com/jackpf/LogScanner.git
cd LogScanner
make
sudo cp ./bin/log-scanner /usr/sbin/
sudo cp ./scripts/init.d/log-scanner /etc/init.d/
sudo cp ./scripts/default/log-scanner /etc/default/
sudo update-rc.d log-scanner defaults
mkdir ~/.config/log-scanner
cp ./config/config.yml ~/.config/log-scanner/
```
Then edit /etc/default/log-scanner and set CONFIG_FILE to point to your config.yml file (just add your username if you've installed as above).

Test it's working with ```sudo service log-scanner start```

Then edit ~/.config/log-scanner/config.yml as necessary.

Valid config.yml entries:

* ```file```: apache log file to watch, eg /var/log/apache2/error.log)
* ```server_name```: the name of the server installed on, or a string you wish to identify notifications by (sent in the title)
* ```pushover_token```: your pushover app token
* ```pushover_user```: pushover user/group id to send notifications to