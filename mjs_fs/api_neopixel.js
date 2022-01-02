load("api_bitbang.js");
load("api_gpio.js");
load("api_sys.js");

let NeoPixel = {
  RGB: 0,
  GRB: 1,
  BGR: 2,

  // ## **`NeoPixel.create(pin, numPixels, order)`**
  // Create and return a NeoPixel strip object. Example:
  // ```javascript
  // let pin = 5, numPixels = 16, colorOrder = NeoPixel.GRB;
  // let strip = NeoPixel.create(pin, numPixels, colorOrder);
  // strip.setPixel(0 /* pixel */, 12, 34, 56);
  // strip.show();
  //
  // strip.clear();
  // strip.setPixel(1 /* pixel */, 12, 34, 56);
  // strip.show();
  // ```
  create: function(pin, numPixels, order) {
    return Object.create({
      _i: NeoPixel._c(pin, numPixels, order),

      setPixel: NeoPixel.setPixel,
      setStrip: NeoPixel.setStrip,
      clear: NeoPixel.clear,
      show: NeoPixel.show,
    });
  },

  // ## **`strip.setPixel(i, r, g, b)`**
  // Set i-th's pixel's RGB value.
  // Note that this only affects in-memory value of the pixel.
  setPixel: function(i, r, g, b) {
    NeoPixel._set(this._i, i, r, g, b);
  },

  // ## **`strip.setStrip(r, g, b)`**
  // Set ALL pixel's RGB value.
  // Note that this only affects in-memory value of the pixels.
  setStrip: function(r, g, b) {
    NeoPixel._setStrip(this._i, r, g, b);
  },

  // ## **`strip.clear()`**
  // Clear in-memory values of the pixels.
  clear: function() {
    NeoPixel._clear(this._i);
  },

  // ## **`strip.show()`**
  // Output values of the pixels.
  show: function() {
    NeoPixel._show(this._i);
  },

  _c: ffi('void *mgos_neopixel_create(int, int, int)'),
  _set: ffi('void mgos_neopixel_set(void *, int, int, int, int)'),
  _clear: ffi('void mgos_neopixel_clear(void *)'),
  _show: ffi('void mgos_neopixel_show(void *)'),
  _setStrip: ffi('void mgos_neopixel_setStrip(void *, int, int, int)')
};
