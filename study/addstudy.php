<?php

$response = array('output' => array());
function addError(&$response, $error) {
    if (!array_key_exists('errors', $response))
        $response += array('errors' => array());
    array_push($response['errors'], $error);
}

if (empty($_REQUEST['name']))
    addError($response, 'Study name is empty');
else if (strpos($_REQUEST['name'], ' ') !== false)
    addError($response, 'Spaces in study name');
else if (strncasecmp($_REQUEST['name'], 'device', strlen('device')) === 0)
    addError($response, 'Study name can\'t start with \'device\'');

if (empty($_REQUEST['description']))
    addError($response, 'Study description is empty');

if (!array_key_exists('branches', $_REQUEST)) {
    addError($response, 'No experiment branches specified');
} else {
    foreach ($_REQUEST['branches'] as $branch)
        if (strpos($branch, ' ') !== false)
            addError($response, 'Branch ' . $branch . ' contains space');
}

if (!array_key_exists('studytype', $_REQUEST)) {
    addError($response, 'No study type specified');
} else if ($_REQUEST['studytype'] !== 'private' && $_REQUEST['studytype'] !== 'public' ) {
    addError($response, 'Unkown study type. Valid: private or public.');
}

if (array_key_exists('errors', $response)) {
    echo(json_encode($response));
    exit(1);
}

$listStudiesCommand = 'admin-scripts/list-studies.sh'
    . ' --ssh-config "/var/www/apex-study/admin-scripts/ssh-config" 2>&1';
$listStudiesOutput = array();
$listStudiesExit = 0;
exec($listStudiesCommand, $listStudiesOutput, $listStudiesExit);

array_push($response['output'], $listStudiesCommand);
$response['output'] = array_merge($response['output'], $listStudiesOutput);
array_push($response['output'], '---');

if ($listStudiesExit != 0) {
    addError($response, 'Unable to list studies, report to admin');
    echo(json_encode($response));
    exit(1);
}

$addStudyCommand = sprintf(
    'admin-scripts/create-study.sh --yes '
    . ' --ssh-config "/var/www/apex-study/admin-scripts/ssh-config"'
    . ' --study %s --description %s --branch %s  2>&1',
    escapeshellarg($_REQUEST['name']),
    escapeshellarg($_REQUEST['description']),
    escapeshellarg($_REQUEST['branches'][0]));

if ($_REQUEST['studytype'] === 'public')
    $addStudyCommand .= ' --public';
else if ($_REQUEST['studytype'] === 'private')
    $addStudyCommand .= ' --private';

if ($_SERVER['Shib-Person-uid'])
    $addStudyCommand .= sprintf(' --committers %s',
                                escapeshellarg($_SERVER['Shib-Person-uid']));
$addStudyOutput = array();
$addStudyExit = 0;
exec($addStudyCommand, $addStudyOutput, $addStudyExit);
array_push($response['output'], $addStudyCommand);
$response['output'] = array_merge($response['output'], $addStudyOutput);
array_push($response['output'], '---');

if ($addStudyExit != 0) {
    addError($response, 'Create-study script failed, see output for details.');
    echo(json_encode($response));
    exit(1);
}

for ($i = 1; $i < count($_REQUEST['branches']); $i++) {
    $addBranchCommand = sprintf(
        'admin-scripts/study-add-branch.sh --yes'
        . ' --ssh-config "/var/www/apex-study/admin-scripts/ssh-config"'
        .' --study %s --branch %s 2>&1',
        escapeshellarg($_REQUEST['name']),
        escapeshellarg($_REQUEST['branches'][$i]));
    $addBranchOutput = array();
    $addBranchExit = 0;
    exec($addBranchCommand, $addBranchOutput, $addBranchExit);
    array_push($response['output'], $addBranchCommand);
    $response['output'] = array_merge($response['output'], $addBranchOutput);
    array_push($response['output'], '---');
    if ($addBranchExit != 0) {
        addError($response, 'Failed to add branch, see output for details.');
    }
}

echo(json_encode($response));

?>
