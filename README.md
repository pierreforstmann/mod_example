# mod_examples
These are module examples from https://httpd.apache.org/docs/2.4/developer/modguide.html slightly modified: I have fixed some compilation warnings and renamed some fonctions and variables so that each module can be compiled and installed at the same time.

## install
```
make
sudo make install
```

## test 

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

<Location "/example4">
    SetHandler example4-handler
</Location>

<Location "/example5">
    SetHandler example5-handler
</Location>

<Location "/example6">
    SetHandler example6-handler
</Location>

```
Create directories and files:
```

mdkir /var/www/html/example2
echo OK > /var/www/html/example2/test.dat

```
Run:
```
curl http://127.0.0.1/example1/
curl http://127.0.0.1/example2/test.dat.sum
curl http://127.0.0.1/example3/
curl http://127.0.0.1/example4/
curl http://127.0.0.1/example5/
curl 'http://127.0.0.1/example6/c?p1=v1&p2=v2'
curl -X POST -d 'p1=v1' -d 'p2=v2' 'http://127.0.0.1/example6/p'

# to test POST from browser

mkdir /var/www/html/example6
cat post.hml
<h4> example6 </h4>
<form action="http://127.0.0.1/example6/post" method="POST">
        <label for="name1">parm1:</label>
	<input type="text" name="parm1"><br><br>
        <label for="name1">parm2:</label>
	<input type="text" name="parm2"><br><br>
	<input type="submit" value="Submit">
</form>

# from browser
file:///var/www/html/example6/post.html

```




