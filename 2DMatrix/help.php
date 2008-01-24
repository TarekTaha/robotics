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

	$helptitle  = DataMatrixPLUGIN_ADMIN_HELP_00;

	$helpcapt[] = DataMatrixPLUGIN_ADMIN_HELP_01;
	$helptext[] = DataMatrixPLUGIN_ADMIN_HELP_02;

//	$helpcapt[] = 2DMatrixPLUGINADMIN_HELP_03;
//	$helptext[] = 2DMatrixPLUGINADMIN_HELP_04;

//	Possible future menu items
//	$helpcapt[] = 2DMatrixPLUGINADMIN_HELP_05;
//	$helptext[] = 2DMatrixPLUGINADMIN_HELP_06;

	$helpcapt[] = DataMatrixPLUGIN_ADMIN_HELP_07;
	$helptext[] = DataMatrixPLUGIN_ADMIN_HELP_08;

	$text2 = "";
	for ($i=0; $i<count($helpcapt); $i++) {
	  $text2 .= "<b>".$helpcapt[$i]."</b><br />";
	  $text2 .= $helptext[$i]."<br /><br />";
	};
  $ns -> tablerender($helptitle, $text2);
?>
