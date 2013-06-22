node-termcast-server
====================

Broadcast your work!

![Preview](https://github.com/jasonmay/node-termcast-server/raw/master/doc/preview.png "node-termcast-server")

Features
--------

#### libvterm

This app is bundled with the libvterm library, which has plenty of awesome terminal emulation support and is wicked fast.

#### 256-color support

Out of the box, thanks to the feature richness of libvterm, node-termcast-server will broadcast the colors very close to the way it looks in your terminal.

#### Terminal Resizing in real-time

The [App::Termcast](http://metacpan.org/module/App::Termcast) client is good about sending resize signals to the server, so when you resize you terminal, you will see the terminal on the web resize in real-time, so there is no need to worry about the dimensions of your terminal!

Installation
------------

```
npm install -g node-gyp
node-gyp rebuild
npm install
```

Running the server
------------------

Make sure you have a termcast server running from here: https://github.com/jasonmay/app-termcast-server

Take note of where the **connections.sock** unix socket is hosted and place the full path in your config:

```
cp config.json.example config.json
<edit config.json>
```

Then you should be good to go.

```
./bin/server
```

TODO
----

Need to track the cursor. The canvas doesn't have anything for this yet.

Need to track background colors.

Need to track other cell attributes, like boldness.

BUGS
----

Oh there are tons of them. Don't get me started. Check the Github issues for anything:

https://github.com/jasonmay/node-termcast-server/issues
