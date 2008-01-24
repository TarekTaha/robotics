<?php
/*
+------------------------------------------------------------------------------+
|       2DMatrix - a plugin by Tarek Taha
|
|	Plugin Support Site: e107.webstartinternet.com
|
|	For the e107 website system visit http://e107.org
|
|	Released under the terms and conditions of the
|	GNU General Public License (http://gnu.org).
+------------------------------------------------------------------------------+
*/
// Protect the file from direct access
if(!defined("e107_INIT")){ exit; }
// Get language file (assume that the English language file is always present)
$lan_file = e_PLUGIN."2DMatrix/languages/".e_LANGUAGE.".php";
include_lan($lan_file);

$sql = new db;
$altCol = false;

$sql -> db_Select("2D_matrices", "*", "id=1");
while($row = $sql-> db_Fetch())
{
    $id   = $row['id'];
    $name = $row['name'];
    $data = $row['data'];
    $type = $row['type'];
    $size = $row['size'];
    $active_status = $row['active_status'];
    $order = $row['order'];
}
if ($size == 'S') 
{
    $s = 4;
}
else if($size == 'M') 
{
    $s = 6;
}
else if($size == 'L') 
{
    $s = 8;
}
else if($size == 'XL') 
{
    $s = 12;
}

// Gather the desired results in the variable $text
$textout .='
	<div style="text-align:center">

				<img src="http://datamatrix.kaywa.com/img.php?s='.$s.'&d='.$data.'" alt="datamatrix"/>

	</div>
';

// Render the value of $text in a table.
$title = $name;
$ns -> tablerender($title, $textout);

?> 
