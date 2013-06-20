var vtchanges = require('../build/Release/vtchanges');

function VT(cols, rows) {
  this.vtchanges = vtchanges.create(rows, cols);
}


VT.prototype = {
  process: function(ansi) {
    return this.vtchanges.process(ansi.toString());
  },
  finish: function() {
    this.vtchanges.finish();
  }
};

module.exports = VT;
