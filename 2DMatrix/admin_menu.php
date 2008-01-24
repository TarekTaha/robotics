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

// class2.php is the heart of e107, always include it first to give access to e107 constants and variables
require_once("../../class2.php");

// Include auth.php rather than header.php ensures an admin user is logged in
require_once(e_ADMIN."auth.php");

// Check to see if the current user has admin permissions for this plugin
if (!getperms("P")) {
	// No permissions set, redirect to site front page
	header("location:".e_BASE."index.php");
	exit;
}

// Get language file (assume that the English language file is always present)
$lan_file = e_PLUGIN."2DMatrix/languages/".e_LANGUAGE.".php";
include_lan($lan_file);

// Set the pageid for the menu as global variable (first pageid is set by admin_config.php)
global $pageid;

$action = basename($_SERVER['PHP_SELF'], ".php");

$var['admin_menu_01']['text'] = DataMatrixPLUGIN_MENU_01;
$var['admin_menu_01']['link'] = "admin_config.php";

//$var['admin_menu_02']['text'] = DataMatrixPLUGIN_MENU_02;
//$var['admin_menu_02']['link'] = "admin_categories.php";

// Put the readme.txt at the end of the list
$var['admin_menu_09']['text'] = DataMatrixPLUGIN_MENU_09;
$var['admin_menu_09']['link'] = "admin_readme.php";

// Show the admin menu with a caption from the language file
$caption = DataMatrixPLUGIN_MENU_00;
show_admin_menu($caption, $pageid, $var);
?>
