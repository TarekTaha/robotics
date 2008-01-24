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
// if e107 is not running we won't run this plugin program
if (!defined('e107_INIT')) { exit; }

// determine the plugin directory as a global variable
global $PLUGINS_DIRECTORY;

// read the database names array of this plugin from the includes/config file
require_once("includes/config.php");

// use this folder during install
$eplug_folder = "2DMatrix";

// Get language file (assume that the English language file is always present)
$lan_file = e_PLUGIN."2DMatrix/languages/".e_LANGUAGE.".php";
include_lan($lan_file);

$eplug_name = DataMatrixPLUGIN_PAGE_NAME;
$eplug_version = "0.1";
$eplug_author = "Tarek Taha";
$eplug_url = "http://www.tarektaha.com";
$eplug_email = "admin@tarektaha.com";
$eplug_description = DataMatrixPLUGIN_DESC;
$eplug_compatible = "e107v0.7+";
$eplug_compliant = TRUE; // indicator if plugin is XHTML compliant, shows icon
$eplug_readme = "readme.txt";

$eplug_menu_name = DataMatrixPLUGIN_MENU;
$eplug_conffile = "admin_config.php";
$eplug_icon = $eplug_folder."/images/logo_32.png";
$eplug_icon_small = $eplug_folder."/images/logo_16.png";
$eplug_caption = DataMatrixPLUGIN_CAPTION;

// List of preferences ---------------------------------------------------------
// this stores a default value(s) in the preferences. 0 = Off , 1= On
// Preferences are saved with plugin folder name as prefix to make preferences unique and recognisable
//$eplug_prefs = "";
  $eplug_prefs = array(
  "datamatrix_version"   => $eplug_version,
  "datamatrix_size"   => array("S","M","L",'XL'),
  "datamatrix_data" => "1",
  "datamatrix_type"   => "Guestbook",
  "datamatrix_name"  => "0"
  );
// List of table names ---------------------------------------------------------
$eplug_table_names = array(
    DB_TABLE_DataMatrixPLUGIN_TABLE
);

// List of sql requests to create tables ---------------------------------------
$eplug_tables = array(

// Apply create instructions for every table you defined in includes\config.php
// MPREFIX must be used because database prefix can be customized instead of default e107_
"CREATE TABLE ".MPREFIX.DB_TABLE_DataMatrixPLUGIN_TABLE." (
  `id` int(10) unsigned NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  `data` varchar(250) NOT NULL default '',
  `type` varchar(30) NOT NULL default '',
  `size` varchar(2) NOT NULL default 'M',
  `active_status` int(11) unsigned NOT NULL default '1',
  `order` int(11) unsigned NOT NULL default '1',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1;", //,
	// As an option: fill the database on first installation with default values,
  // otherwise slash out INSERT INTO line just to create an empty database file structure.
	// In case you slash out line 81, also slash out the comma in above line 77.
    "INSERT INTO ".MPREFIX.DB_TABLE_DataMatrixPLUGIN_TABLE." VALUES ('1', 'WebSite Link', 'http://www.tarektaha.com', 'link', 'M', '1', '1')"
);

// Create a link in main menu (yes=TRUE, no=FALSE) -------------------------------------------------------------------
$eplug_link = FALSE;
$eplug_link_name = DataMatrixPLUGIN_LINK_NAME;
// $plugins_directory can be named differently than the default e107_plugins in the e107_config
$eplug_link_url = $PLUGINS_DIRECTORY."2DMatrix/2DMatrix.php";
$eplug_done = DataMatrixPLUGIN_DONE1." ".$eplug_name." v".$eplug_version." ".DataMatrixPLUGIN_DONE2;

// upgrading ... //
$upgrade_add_prefs = "";
$upgrade_remove_prefs = "";
//$upgrade_alter_tables = "";

//*
$upgrade_alter_tables = array(

	"",
	
	""
    );
    
//*/

$eplug_upgrade_done = DataMatrixPLUGIN_UPGRADE_DONE." ".$eplug_name." v".$eplug_version.".";
?>
