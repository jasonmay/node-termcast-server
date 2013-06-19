var createVTchanges = require('../build/Release/vtchanges');

function VT(cols, rows) {
  this.vtchanges = createVTchanges(rows, cols);
}


VT.prototype = {
  process: function(ansi) {
    console.log(ansi.toString());
    return this.vtchanges.process(ansi.toString());
  },
  finish: function() {
    this.vtchanges.finish();
  }
};

module.exports = VT;
