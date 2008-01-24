<?php
/*
+------------------------------------------------------------------------------+
|     YourFirstPlugin - a plugin skeleton by nlstart
|
|	Plugin Support Site: e107.webstartinternet.com
|
|	For the e107 website system visit http://e107.org
|
|	Released under the terms and conditions of the
|	GNU General Public License (http://gnu.org).
+------------------------------------------------------------------------------+
*/
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

// Set the active menu option for admin_menu.php
$pageid = 'admin_menu_01';
// If the submit button is hit; update the settings in table core, record SitePrefs
// Initial default values of the preferences are set by plugin.php at $eplug_prefs
$sql -> db_Select(DB_TABLE_DataMatrixPLUGIN_TABLE, "*", "id=1");
while($row = $sql-> db_Fetch()){
    $id   = $row['id'];
    $name = $row['name'];
    $data = $row['data'];
    $type = $row['type'];
    $size = $row['size'];
    $active_status = $row['active_status'];
    $order = $row['order'];
}
if (isset($_POST['update_prefs'])) {
    $sql -> db_Update(DB_TABLE_DataMatrixPLUGIN_TABLE,"name='".$tp->toDB($_POST['name'])."',
		type='".$tp->toDB($_POST['type'])."',
		data='".$tp->toDB($_POST['data'])."',
		size='".$tp->toDB($pref['datamatrix_size'][$_POST['size']])."'
		WHERE id=1");
	$message = DataMatrixPLUGIN_CONF_11;
}
// displays the update message to confirm data is stored in database
if (isset($message)) {
	$ns->tablerender("", "<div style='text-align:center'><b>".$message."</b></div>");
}

// The following form output will be put into variable $text.
// The form will call admin_config_edit for further actions.
// All language dependent text are referring to the language file.
// E.g. .DataMatrixPLUGIN_CONF_01. will retrieve the accompanying text.
// Remember NOT to put comments in the text as they will be published.
// In the form data is retrieved from table core, record SitePrefs using $pref
$text .= '
<div style="text-align:center">
<form name="settings_form" action="'.e_SELF.'" method="post">
	<fieldset>
		<legend>
			'.DataMatrixPLUGIN_CONF_01.'
		</legend>
		<table border="0" class="tborder" cellspacing="10">
			<tr>
				<td class="tborder" style="width: 200px">
					<span class="smalltext" style="font-weight: bold">
						'.DataMatrixPLUGIN_CONF_02.'
					</span>
				</td>
				<td class="tborder" style="width: 200px">
					<input class="tbox" size="25" type="text" name="name" value="'.$name.'" />
				</td>
			</tr>
			<tr>
    <td class="tborder" style="width: 200px">
     <span class="smalltext" style="font-weight: bold">
						'.DataMatrixPLUGIN_CONF_03.'
					</span>
				</td>
    <td class="tborder" style="width: 200px" valign="top">
					<input class="tbox" size="35" type="text" name="data" value="'.$data.'" />
				</td>
			</tr>
			<tr>
    <td class="tborder" style="width: 200px">
     <span class="smalltext" style="font-weight: bold">
						'.DataMatrixPLUGIN_CONF_05.'
					</span>
				</td>
    <td class="tborder" style="width: 200px">
					<input class="tbox" size="25" type="text" name="type" value="'.$type.'" />
				</td>
			</tr>
			<tr>
    <td class="tborder" style="width: 200px">
     <span class="smalltext" style="font-weight: bold">
						'.DataMatrixPLUGIN_CONF_09.'
					</span>
				</td>
    <td class="tborder" style="width: 200px">
					<select name="size" class="tbox"><option value="0">'.$size.'</option>"
					<option value="0">'.$pref['datamatrix_size'][0].'</option>"
					<option value="1">'.$pref['datamatrix_size'][1].'</option>"
					<option value="2">'.$pref['datamatrix_size'][2].'</option>"
					<option value="3">'.$pref['datamatrix_size'][3].'</option>"
					</select>
				</td>
			</tr>
		</table>
	</fieldset>
	<br />
	<input class="button" type="submit" name="update_prefs" value="'.DataMatrixPLUGIN_CONF_10.'">
	<br />
</form>
</div>
';
//<input class="tbox" size="25" type="text" name="size" value="'.$pref['size'].'" />

// Display the $text string into a rendered table with a caption from the language file
$caption = DataMatrixPLUGIN_CONF_00;
$ns->tablerender($caption, $text);

require_once(e_ADMIN."footer.php");
?>
