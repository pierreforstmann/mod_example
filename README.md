# mod_example
https://httpd.apache.org/docs/2.4/developer/modguide.html

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
Run:
```
mkdir /var/www/html/example1
mdkir /var/www/html/example2
echo OK > /var/www/html/example2/test.dat
mkdir /var/www/html/exampleA
touch /var/www/html/exampleA/exampleA.txt
mkdir /var/www/html/example3
```




