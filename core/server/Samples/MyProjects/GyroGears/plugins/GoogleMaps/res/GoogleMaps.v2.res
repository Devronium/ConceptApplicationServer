<!DOCTYPE html>
<html lang="en">
  <head>
	<meta name="viewport" content="initial-scale=1.0, user-scalable=no" />
	<meta http-equiv="content-type" content="text/html; charset=UTF-8"/>
	<style type="text/css">
		html, body {
		  height: 100%;
		  margin: 0;
		  padding: 0;
		}

		#map_canvas {
		  height: 100%;
		}

		@media print {
		  html, body {
			height: auto;
		  }

		  #map_canvas {
			height: 650px;
		  }
		}
	</style>
    <script src="http://maps.google.com/maps?file=api&amp;v=2.x&amp;key=[KEY]" type="text/javascript"></script>
    <script type="text/javascript">
    var map = null;
    var geocoder = null;

    function showAddress(address) {
      if (geocoder) {
        geocoder.getLatLng(
          address,
          function(point) {
            if (!point) {
              alert(address + " not found");
            } else {
              map.setCenter(point, 13);
              var marker = new GMarker(point);
              map.addOverlay(marker);
              marker.openInfoWindow("[DESCRIPTION]");
            }
          }
        );
      }
    }
    
    function initialize() {
      if (GBrowserIsCompatible()) {
        map = new GMap2(document.getElementById("map_canvas"));
        map.setCenter(new GLatLng(0,0), 8, G_HYBRID_MAP);
        map.setMapType(G_HYBRID_MAP);
        map.setUIToDefault();
        map.enableRotation();
        geocoder = new GClientGeocoder();
      }
      showAddress("[ADDRESS]");
    }
    </script>
  </head>

  <body onload="initialize()" onunload="GUnload()">
	<div id="map_canvas"></div>
  </body>
</html>
