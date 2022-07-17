# mod_examples
These are module examples from https://httpd.apache.org/docs/2.4/developer/modguide.html slightly modified: I have fixed some compilation warnings and renamed some fonctions and variables so that each module can be compiled and installed at the same time.

## to install
```
make
sudo make install
```

## to test 

Add to httpd.conf:

```
<Location "/exampleA">
    SetHandler exampleA-handler
</Location>

<Location "/example2">
    SetHandler example2-handler
</Location>

<Location "/example3">
    SetHandler example3-handler
</Location>

<Location "/example1">
    SetHandler example1-handler
</Location>
```
Create directories and files
```
mkdir /var/www/html/example1
mdkir /var/www/html/example2
echo OK > /var/www/html/example2/test.dat
mkdir /var/www/html/exampleA
touch /var/www/html/exampleA/exampleA.txt
mkdir /var/www/html/example3
```
Run
```
curl http://127.0.0.1/example1/
curl http://127.0.0.1/example2/test.dat.sum
curl http://127.0.0.1/example3/
```




