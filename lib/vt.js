var vtchanges = require('../build/Release/vtchanges');

function VT(cols, rows) {
  this.vtchanges = vtchanges.create(rows, cols);
}


VT.prototype = {
  process: function(ansi) {
    var data = this.vtchanges.process(ansi.toString('utf8'));
    //if (data && data[0]) console.dir(data[0][2]); // testing unicode chars
    //if (data && data[1]) console.dir(data[1][2]); // testing unicode chars
    return data;
  },
  snapshot: function() {
    return this.vtchanges.snapshot();
  },
  resize: function(cols, rows) {
    return this.vtchanges.resize(rows, cols);
  },
  finish: function() {
    this.vtchanges.finish();
  }
};

module.exports = VT;
