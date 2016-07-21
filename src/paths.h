static const GPathInfo HOUR_HAND_PATHINFO = {
   .num_points = 4,
   .points = (GPoint []) {
      //X, Y
      {-4, 0},
      {4, 0},
      {4, -54},
      {-4, -54}
   }
};

static const GPathInfo MINUTE_HAND_PATHINFO = {
   .num_points = 4,
   .points = (GPoint []) {
      //X, Y
      {-3, 0},
      {3, 0},
      {3, -72},
      {-3, -72}
   }
};

/* DEPRECATED:
   Paths are cool but not when they're rotated and skinny,
   unfortunately Pebble doesn't antialias them. :(
static const GPathInfo SECOND_HAND_PATHINFO = {
   .num_points = 9,
   .points = (GPoint []) {
      //X, Y
      {-4, 15},
      {0, 20},
      {4, 15},
      {4, 0},
      {3, 0},
      {3, -72},
      {-3, -72},
      {-3, 0},
      {-4, 0}
   }
};
*/

static const GPathInfo CHEVRON_PATHINFO = {
   .num_points = 6,
   .points = (GPoint []) {
      //X, Y
      {0, 0},
      {11, 3},
      {11, -2},
      {0, -5},
      {-10, -2},
      {-10, 3}
   }
};