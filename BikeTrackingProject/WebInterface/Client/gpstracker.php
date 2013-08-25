<?php

$con = mysql_connect("localhost","username","password");
if (!$con)
{
	die('Could not connect: ' . mysql_error());
}
mysql_select_db("database_name", $con);

if(!empty($_GET))
{
 $speed = $_GET["speed"];
 $lat   = $_GET["lat"];
 $lng   = $_GET["lng"];
 $date  = time();
 mysql_query("INSERT INTO tracker (date, speed, lat, lng, course) VALUES ('$date', '$speed','$lat','$lng', 'SSE')");
 $text = " Added Row !!!";
}
            
// AIzaSyBvacjxfjOsv5f4l2cRu7gcmH32f8IKtP0
$result = mysql_query("SELECT * FROM tracker");

$locs = "";
$started = 0;
while($row = mysql_fetch_array($result))
{
  if($started==1)
    $locs .=",";
  else
    $started = 1; 
  $locs .= '['.$row["id"].', '.$row["lat"].', '.$row["lng"].', "'.$row["date"].'", "'.$row["speed"].'", "'.$row["course"].'"]';
}
$text .= "<html>
  <head>
    <title>Google Maps JavaScript API v3 Example: Map Simple</title>
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">
    <meta charset=\"utf-8\">
    <style>
      html, body, #map_canvas {
        margin: 0;
        padding: 0;
        height: 100%;
      }
    </style>
    <script src=\"https://maps.googleapis.com/maps/api/js?sensor=false\"></script>
    <script  type=\"text/javascript\">
      var locations = new Array (".$locs.");
    //initialise the infowindow so it's ready for use
    var infowindow = null;

    //function to actually initialise the map
    function initialize() 
    {
        //set your latitude and longitude as per the previous article, specify the default 
        //zoom and set the default map type to RoadMap
        var latlng = new google.maps.LatLng(locations[locations.length-1][1], locations[locations.length-1][2]);
        var myOptions = {
            zoom: 12,
            center: latlng,
            mapTypeId: google.maps.MapTypeId.ROADMAP
        };
        //place the map on the canvas
        var map = new google.maps.Map(document.getElementById('map_canvas'), myOptions);
        var infowindow = new google.maps.InfoWindow();
        //var bounds = new google.maps.LatLngBounds();
        //Set all your markers, the magic happens in another function - setMarkers(map, markers) which gets called
        //loop through and place markers
	var marker;
	var siteLatLng,sites;
        for (var i = 0; i < locations.length; i++) 
        {
            sites = locations[i];
            siteLatLng = new google.maps.LatLng(sites[1], sites[2]);
            marker = new google.maps.Marker({
                position: siteLatLng,
                map: map,
		animation: google.maps.Animation.DROP
            });
            //attach infowindow on click
	    google.maps.event.addListener(marker, 'click', (function(marker, i) {
	      return function() {
		infowindow.setContent(locations[i][0]);
		infowindow.open(map, marker);
	      }
	    })(marker, i));
        }
	//map.fitBounds(bounds);
    }
    </script>
</head>
  <body>
    <div id='map_canvas'></div>
    <script type='text/javascript'>initialize();</script>
  </body>
</html>";
print $text;
mysql_close($con);
?> 
