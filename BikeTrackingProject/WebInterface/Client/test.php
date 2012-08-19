<?php

 $speed = $_GET["speed"];
 $lat   = $_GET["lat"];
 $lng   = $_GET["lng"];
 $date  = time();

$con = mysql_connect("orf-mysql1.brinkster.com","tarektaha","a3shaqoha_Flstn");
if (!$con)
{
	die('Could not connect: ' . mysql_error());
}

mysql_select_db("tarektaha", $con);

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
$text = "<html>
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
        var latlng = new google.maps.LatLng(-32.8333, 115.917);
        var myOptions = {
            zoom: 10,
            center: latlng,
            mapTypeId: google.maps.MapTypeId.ROADMAP
        };
        
        //place the map on the canvas
        var map = new google.maps.Map(document.getElementById(\"map_canvas\"), myOptions);
        
        //Set all your markers, the magic happens in another function - setMarkers(map, markers) which gets called
        setMarkers(map, locations);
        infowindow = new google.maps.InfoWindow({
        content: \"holding...\"
        });
    }  
    //function to actually put the markers on the map
    function setMarkers(map, markers) 
    {
        //loop through and place markers
        for (var i = 0; i < markers.length; i++) 
        {
            var sites = markers[i];
            var siteLatLng = new google.maps.LatLng(sites[1], sites[2]);
            var marker = new google.maps.Marker({
                position: siteLatLng,
                map: map,
                title: sites[0],
                zIndex: sites[3],
                html: sites[4]
            });

            //initial content string
            var contentString = \"Some content\";

            //attach infowindow on click
            google.maps.event.addListener(marker, \"click\", function () 
            {
                infowindow.setContent(this.html);
                infowindow.open(map, this);
            });
        }
    }
    </script>
</head>
  <body>
    <div id=\"map_canvas\"></div>
    <script type=\"text/javascript\">initialize();</script>
  </body>
</html>";
print $text;
mysql_close($con);
?> 
