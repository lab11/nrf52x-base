#!/usr/bin/env node

var coap                   = require('coap');
var server                 = coap.createServer();

server.on('request', function(req, res) {
  try {
  var date = new Date().toISOString();
  console.log(date);
  console.log(req.url);
  console.log(req.payload);
  console.log(req.options);


  var blockOption = undefined
  for(var i = 0; i < req.options.length; i++) {
    if (req.options[i].name == 'Block1')
    {
      blockOption = req.options[i].value;
    }
  }
  console.log(blockOption);
  if ((blockOption.readUIntBE(0, blockOption.length) & 0x8) >> 3) {
    res.code = 231
  } else {
    res.code = 204;
  }
  res.setOption('Block1', blockOption);
  res.end();
  }
  catch (e) {
    console.log(e);
  }
});

server.listen(function() {
    console.log("Listening");
});


