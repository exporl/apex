<?php

$response = array('branches' => array());
function addError(&$response, $error) {
    if (!array_key_exists('errors', $response))
        $response += array('errors' => array());
    array_push($response['errors'], $error);
}
if (empty($_REQUEST['study']))
    addError($response, 'No study specified');
if (array_key_exists('errors', $response)) {
    echo(json_encode($response));
    exit(1);
}

$listBranchesCommand = sprintf('admin-scripts/list-branches.sh'
                                 . ' --ssh-config "/var/www/apex-study/admin-scripts/ssh-config"'
                                 . ' --study %s', escapeshellarg($_REQUEST['study']));
$response['branches'] = array_filter(explode("\n", shell_exec($listBranchesCommand)));

echo(json_encode($response));
?>
