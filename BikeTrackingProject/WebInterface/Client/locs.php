<?php
/*
+---------------------------------------------------------------+
|        Map Me! for e107 v0.7
|
|        A plugin for the e107 website system
|        http://www.e107.org/
|
|        Ajay Bhargav
|        http://www.rickeyworld.info
|        contact@rickeyworld.info
|
|        Special thanks to nlstart for bugfixes
|
|        Released under the terms and conditions of the
|        GNU General Public License (http://gnu.org).
+---------------------------------------------------------------+
*/

	require_once("class2.php");
	header("Content-type: application/x-javascript");
	$userid = $_GET["userid"];
	global $pref;
	$sql->mySQLresult = @mysql_query("SELECT s.MapID as map_id, s.Latitude, s.Longitude, s.Village, s.District, v.village_arabic_name FROM ".MPREFIX."palestinian_villages_stats as s LEFT JOIN ".MPREFIX."palestinian_villages as v on s.MapID=v.MapID where s.Longitude!=0 and s.Latitude!=0 and s.MapID!=0 order by s.District, s.Village");
	$rows=$sql->db_Rows();
	$userloc = "";
	if($rows){
 		for($i=0;$i<$rows;$i++){
 		 	$row = $sql->db_Fetch();
			$userloc .= '['.$row["Latitude"].', '.$row["Longitude"].', '.$row["map_id"].', "'.$row["Village"].'", "'.$row["District"].'", "'.$row["village_arabic_name"].'"]';
			if($i!=$rows-1)
				$userloc .=",";
		}
	}
		$ret = "
		//<![CDATA[
		
		var userloc = new Array (".$userloc.");
		var bounds = new GLatLngBounds();
		function createSidebarEntry(marker, name, address) 
		{
		  var div = document.createElement('div');
		  var html = '' + address + ' - ' + name;
		  div.innerHTML = html;
		  div.style.cursor = 'pointer';
		  div.style.marginBottom = '5px';
		  GEvent.addDomListener(div, 'click', function(){ GEvent.trigger(marker, 'click');});
		  GEvent.addDomListener(div, 'mouseover', function() {div.style.backgroundColor = '#eee';});
		  GEvent.addDomListener(div, 'mouseout', function() {div.style.backgroundColor = '#fff';});
		  return div;
		}
		
		function createMarker(point, id, name, loc,arabic_name)
		{
			var marker = new GMarker(point, Icon);
			GEvent.addListener(marker, 'click', function() {
				marker.openInfoWindowHtml('<a href=\"http://www.palestiniansunited.org/palgroup/palestinian_villages/index.php?cmd=add&id='+ id +'&userid=' + $userid + '\"><b>'+ name +' '+arabic_name+'    </b></a><br />District: '+ loc);
			});
			return marker;
		}
		
		function add_user(){
			for (var i = 0; i < userloc.length; i++) {
				var point = new GLatLng(userloc[i][0], userloc[i][1]);
				var marker = createMarker(point, userloc[i][2], userloc[i][3], userloc[i][4],userloc[i][5]);
				map.addOverlay(marker);
      				var sidebarEntry = createSidebarEntry(marker, userloc[i][3], userloc[i][4]);
				sidebar.appendChild(sidebarEntry);
				bounds.extend(point);
			}
		}
		
		////map
		var sidebar = document.getElementById('mapsidebar');
	        sidebar.innerHTML = '';
		var map = new GMap2(document.getElementById('map'));
		map.enableScrollWheelZoom();
		map.setCenter(new GLatLng(31.587894,35.870361), 7);
		map.addControl(new GMapTypeControl());
		map.addControl(new GLargeMapControl());

		map.enableContinuousZoom();
		map.enableDoubleClickZoom();

		var ovSize=new GSize(100, 100);
		var ovMap=new GOverviewMapControl(ovSize);
		map.addControl(ovMap);
		var mini=ovMap.getOverviewMap();
	
		map.hideControls();
		GEvent.addListener(map, 'mouseover', function(){
			map.showControls();
		});
		GEvent.addListener(map, 'mouseout', function(){
			map.hideControls(); 
		});

		var Icon = new GIcon();
		Icon.image = 'images/red.png';
		Icon.shadow = 'images/shadow.png';
		Icon.iconSize = new GSize(12, 20);
		Icon.shadowSize = new GSize(22, 20);
		Icon.iconAnchor = new GPoint(6, 20);
		Icon.infoWindowAnchor = new GPoint(5, 1);
		add_user();
	
		//]]>
		";
	echo $ret;
?>
