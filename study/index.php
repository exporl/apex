<!DOCTYPE html>
<html lang="en">
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title></title>
    <link href="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm" crossorigin="anonymous">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jqueryui/1.12.1/jquery-ui.min.js"></script>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/jqueryui/1.12.1/jquery-ui.min.css" />
    <script type="text/javascript">
      'use strict'
      $(document).ready(function() {
          $('#addStudy').hide();
          $('#devices').hide();
          $('#linkDeviceToStudy').hide();
          resetForms();
          showOverview();
          <?php
          if (array_key_exists('publicKey', $_REQUEST)) {
              echo('showDevices();');
              echo('$(\'#addDevice-pubkey\').val(\'' . rawurldecode($_REQUEST['publicKey']) . '\');');
          }
          ?>
      });

      function resetForms() {
          $('input').each(function(index) {
              $(this).val('');
          });
          $('textarea').each(function(index) {
              $(this).val('');
          });
      }

      function hideAll() {
          resetForms();
          $('#studies').hide();
          $('#devices').hide();
          $('#linkDeviceToStudy').hide();
          $('#overview').hide();
          $('#addStudy').hide();
      }

      function showOverview() {
          hideAll();
          $('#overview').show();
          $('#overview-generated-url').hide();
          $.get('listdevices.php', function(data) {
              var parsed = JSON.parse(data);
              $('#overview-devices').autocomplete({
                  source: parsed['personal'].concat(parsed['remaining'])
              });
          });
          $.get("liststudies.php", function(data) {
              var parsed = JSON.parse(data);
              $('#overview-studies').autocomplete({
                  source: parsed
              });
          });
          var maxHeight = $('.overview-card').first().outerHeight();
          $('.overview-card').each(function(index) {
              if ($(this).height() > maxHeight)
                  maxHeight = $(this).outerHeight();
          });
          $('.overview-card').css('min-height', maxHeight);
      }

      function preFormPost(outputId, errorsId, successId, data) {
          $(errorsId).empty();
          $(outputId).empty();
          $(errorsId).hide();
          $(outputId).hide();
          $(successId).hide();
          $(errorsId).append('<div class="row"><h6>Errors:</h6></div>');
          $(outputId).append('<div class="row"><h6>Output:</h6></div>');
          $('.submitbutton').prop('disabled', true);
      }

      function parseFormOutput(outputId, errorsId, successId, data) {
          var parsed = JSON.parse(data);
          if (parsed.hasOwnProperty('errors')) {
              $(errorsId).show();
              $.each(parsed['errors'], function(index, value) {
                  $(errorsId).append(
                      '<div class="row">' + value + '</div>');
              });
          } else {
              $(successId).show();
          }
          if (parsed.hasOwnProperty('output')) {
              $(outputId).show();
              $.each(parsed['output'], function(index, value) {
                  $(outputId).append(
                      '<div class="row">' + value + '</div>');

              });
          }
          $('.submitbutton').prop('disabled', false);
      }

      function showStudies() {
          hideAll();
          $('#studies').show();
          $('#studies-showAddStudy').show();
          $('#study-deviceslist').hide()
          $('#studies-list').empty();
          $.get('liststudies.php', function(data) {
              var parsed = JSON.parse(data).sort(function(a, b) {
                  var loweredA = a.toLowerCase(),
                      loweredB = b.toLowerCase();
                  return loweredA > loweredB ? 1 : loweredA < loweredB ? -1 : 0;
            });
            $('#studies-list>select').empty();
            $.each(parsed, function(index, value) {
              $('#studies-list').append('<option>' + value + '</option>');
            });
          });
      }

      function listStudyDevices(study) {
          var payload = {};
          $('#study-deviceslist').hide()
          $('#study-deviceslist>tbody').empty();
          payload['study'] = study;
          $.post('listdevices.php', payload, function(data) {
              var parsed = JSON.parse(data);
              $.each(parsed['personal'].concat(parsed['remaining']), function(index, value) {
                  var url = 'ssh://' + value + '@exporl-ssh.med.kuleuven.be:8444/' + study;
                  $('#study-deviceslist>tbody')
                      .append('<tr>'
                              +'<td>' + value + '</td>'
                              +'<td><button class="btn btn-sm" type="button" onclick="copyToClipboard(\'' + url + '\')">&#x1F4CB;</button>&nbsp;<tt>'
                              + url
                              + '</tt></td></tr>');
              });
              $('#study-deviceslist').show()
          });
      }

      function copyToClipboard(text) {
          navigator.clipboard.writeText(text);
      }

      function showEditStudy(study) {
          resetForms();
          $('#addStudy').show();
          $('#studies-showAddStudy').show();
          $('#addStudy-studyName').val(study);
          $('#addStudy-studyName').prop('disabled', true);
          $('#addStudy > form > .form-group:nth-child(3)').hide();
          $('#addStudy-studyDescription').val(study);
      }

      function showAddStudy() {
          resetForms();
          $('#addStudy').show();
          $('#studies-showAddStudy').hide();
          $('#addStudy-studyName').prop('disabled', false);
          $('#addStudy > form > .form-group').show();
          $('#addStudy>button').show();
          $('#addStudy-visitGerrit').show();
          $('#addStudy>form').hide();
          $('#addStudy>button').hide();
          checkStudyName();
          $.get('account-exists.php', function(data) {
              var exists = JSON.parse(data)[0] === 'true';
              if (exists) {
                  $('#addStudy-visitGerrit').hide();
                  $('#addStudy>form').show();
                  $('#addStudy>button').show();
              }
          });
      }

      function checkStudyName() {
          var currentYear = (new Date()).getFullYear();
          if (!$('#addStudy-studyName').val().startsWith(String(currentYear)))
              $('#addStudy-studyName').val(
                  currentYear + '-' + $('#addStudy-studyName').val());
      }

      function addStudyAddBranchInput() {
          $('<div id="addStudy-branches" class="addStudy-branches form-group">'
            + '<label for="addStudy-branchName">Experiment branch name:</label>'
            + '<input id="addStudy-branchName" class="form-control" type="text"/>'
            + '</div>').insertAfter($('.addStudy-branches').last());
      }

      function addStudy() {
          var outputId = '#addStudy-output',
              errorsId = '#addStudy-errors',
              successId = '#addStudy-success',
              payload = {};
          preFormPost(outputId, errorsId, successId);
          payload['name'] = $('#addStudy-studyName').val();
          payload['description'] = $('#addStudy-studyDescription').val();
          payload['branches'] = [];
          payload['studytype'] = $('#addStudy-publicRadio').is(':checked') ?
              'public' : $('#addStudy-privateRadio').is(':checked') ? 'private' : '';
          $('.addStudy-branches>input').each(function(index) {
              if ($(this).val() !== '')
                  payload['branches'].push($(this).val());
          });
          $.post('addstudy.php', payload, function(data) {
              parseFormOutput(outputId, errorsId, successId, data);
              if ($(successId).is(':visible')) {
                  $('#addStudy>form').hide();
                  $('#addStudy>button').hide();
              }
          });
      }

      function showDevices() {
          hideAll();
          $('#devices').show();
          $('#devices-list>tbody').empty();
          $.get('listdevices.php', function(data) {
              var parsed = JSON.parse(data);
              $.each(parsed['personal'].concat(parsed['remaining']), function(index, value) {
                  $('#devices-list>tbody').append('<tr><td>' + value + '</td></tr>');
              });
          });
      }

      function addDevice() {
          var outputId = '#addDevice-output',
              errorsId = '#addDevice-errors',
              successId = '#addDevice-success',
              payload = {};
          preFormPost(outputId, errorsId, successId);
          payload['pubkey'] = $('#addDevice-pubkey').val();
          $.post("adddevice.php", payload, function(data) {
              parseFormOutput(outputId, errorsId, successId, data);
          });
      }

      function showLinkDeviceToStudy() {
          hideAll();
          $('#linkDeviceToStudy').show();
          $('#linkDeviceToStudy-devices').empty();
          $('#linkDeviceToStudy-studies').empty();
          $.get('listdevices.php', function(data) {
              var parsed = JSON.parse(data);
              parsed['personal'].forEach(function(value) {
                  $('#linkDeviceToStudy-devices').append(
                      '<option class="device-personal">'
                          + value + '</option>');
              });
              parsed['remaining'].forEach(function(value) {
                  $('#linkDeviceToStudy-devices').append(
                      '<option class="device-remaining">' + value + '</option>');
              });
              if (parsed['personal'].length > 0)
                  linkDeviceToStudyShowPersonalDevices();
          });
          $.get('liststudies.php', function(data) {
              var parsed = JSON.parse(data);
              parsed.forEach(function(value) {
                  $('#linkDeviceToStudy-studies').append(
                      '<option>' + value + '</option>');
              });
              linkDeviceToStudyFillExperimentBranches();
          });
      }

      function linkDeviceToStudyShowPersonalDevices() {
          $('.device-remaining').hide();
          $('.device-personal').show();
      }

      function linkDeviceToStudyShowRemainingDevices() {
          $('.device-remaining').show();
          $('.device-personal').show();
      }

      function linkDeviceToStudyFillExperimentBranches() {
          $('#linkDeviceToStudy-branches').empty();
          var payload = {};
          payload['study'] = $('#linkDeviceToStudy-studies').val();
          $.post('listbranches.php', payload, function(data) {
              var parsed = JSON.parse(data);
              if (parsed.hasOwnProperty('branches')) {
                  parsed['branches'].forEach(function(value) {
                      $('#linkDeviceToStudy-branches').append(
                          '<option>' + value + '</option>');
                  });
              }
          });
      }

      function connectDeviceToStudy() {
          var outputId = '#connectDeviceToStudy-output',
              errorsId = '#connectDeviceToStudy-errors',
              successId = '#connectDeviceToStudy-success',
              payload = {};
          preFormPost(outputId, errorsId, successId);
          payload['devices'] = $('#linkDeviceToStudy-devices').val();
          payload['study'] = $('#linkDeviceToStudy-studies').val();
          payload['branches'] = $('#linkDeviceToStudy-branches').val();
          $.post('connectdevicetostudy.php', payload, function(data) {
              parseFormOutput(outputId, errorsId, successId, data);
          });
      }
      </script>
  </head>

    <body>
    <div id="overview">
      <div class="container mt-3">
        <div class="page-header">
          <h1>Apex Study management page</h1>
        </div>
        <div class="lead mt-3">
          This webpage provides an interface for the
          <a id="experiments-url" href="https://exporl.med.kuleuven.be/apex-experiments">
            <u>https://exporl.med.kuleuven.be/apex-experiments</u>
          </a>
                <?php
       ?>

          gerrit server. It can create new studies or add experiment branches to existing ones,
          register devices and link registered devices to studies.
        </div>
        <div class="row mt-5 row-eq-height">
          <div class="col">
            <div class="card-header overview-card">
              View and add studies. Generate urls for devices connected to a study.
            </div>
            <button onclick="showStudies()" class="btn btn-outline-info btn-block">
              Studies
            </button>
          </div>
          <div class="col">
            <div class="card-header overview-card">
              View and add devices.
            </div>
            <button onclick="showDevices()" class="btn btn-outline-info btn-block">
              Devices
            </button>
          </div>
          <div class="col">
            <div class="card-header overview-card">
              Connect devices to a study.
            </div>
            <button onclick="showLinkDeviceToStudy()" class="btn btn-outline-info btn-block">
              Link device to study
            </button>
          </div>
        </div>
      </div>
    </div>

    <div id="studies">
      <div class="container pt-3">
        <div class="row">
          <button onclick="showOverview()" class="btn btn-outline-info col-2">
            &larr;
          </button>
          <div class="col-10 text-center">
            <h4>Studies</h4>
          </div>
        </div>
        <div class="mt-3">
          <div class="row">
            <div class="col-10">
              <div class="alert alert-info" role="alert">
                Click on a study to generate urls.
              </div>
            </div>
            <div class="col-2">
              <button id="studies-showAddStudy" class="btn btn-outline-secondary"
                      type="button" onclick="showAddStudy()">
                Add study
              </button>
            </div>
          </div>
          <form>
            <div class="form-group">
              <select id="studies-list" class="form-control" size="20"
                  onchange="listStudyDevices(this.value); showEditStudy(this.value);">
              </select>
            </div>
          </form>
          <div id="addStudy">
            <div id="addStudy-visitGerrit" class="alert alert-info mt-1" role="alert">
              Please visit
              <a href="https://exporl.med.kuleuven.be/apex-experiments">
                https://exporl.med.kuleuven.be/apex-experiments
              </a>
              (only needed once) before creating a study to ensure you're owner of said study.
            </div>
            <form>
              <div class="form-group">
                <label for="addStudy-StudyName">Study name:</label>
                <input id="addStudy-studyName" class="form-control"
                       type="text" onchange="checkStudyName()"/>
              </div>
              <div class="form-group">
                <label>Study type:</label>
                <label class="form-check-label radio-inline mr-3 ml-3">
                  <input type="radio" name="addStudy-studyType" id="addStudy-publicRadio"> public
                </label>
                <label class="form-check-label radio-inline">
                  <input type="radio" name="addStudy-studyType" id="addStudy-privateRadio"> private
                </label>
                <div class="alert alert-info mt-1" role="alert">
                  Public studies forbid devices from uploading results. Select private for that functionality.
                </div>
              </div>
              <div class="form-group">
                <label for="addStudy-StudyDescription">Study description:</label>
                <input id="addStudy-studyDescription" class="form-control" type="text"/>
              </div>
              <div id="addStudy-branches" class="addStudy-branches form-group">
                <label for="addStudy-branchName">Experiment branch name:</label>
                <input id="addStudy-branchName" class="form-control" type="text"/>
              </div>
              <div class="form-group">
                <button id="addStudy-addBranchButton" type="button"
                        class="btn btn-light" onclick="addStudyAddBranchInput()">
                  + add branch
                </button>
              </div>
            </form>
            <button class="btn btn-primary submitbutton" type="button" onclick="addStudy()">
              Submit
            </button>
            <button class="btn btn-outline-secondary submitbutton" type="button" onclick="showStudies()">
              Cancel
            </button>
            <div id="addStudy-errors"
                 class="alert alert-danger mt-3" class="alert" style="display: none;">
            </div>
            <div id="addStudy-success"
                 class="alert alert-success mt-3" class="alert" style="display: none;">
              <h5>Study created</h5>
            </div>
            <div id="addStudy-output"
                 class="alert alert-dark mt-3" role="alert" style="display: none;">
            </div>
          </div>
          <table id="study-deviceslist" class="table mt-3">
            <thead>
              <tr>
                <th>Device</th>
                <th>Url</th>
              </tr>
            </thead>
            <tbody>
            </tbody>
          </table>
        </div>
      </div>
    </div>

    <div id="devices">
      <div class="container pt-3">
        <div class="row">
          <button onclick="showOverview()" class="btn btn-outline-info col-2">
            &larr;
          </button>
          <div class="col-10 text-center">
            <h4>Devices</h4>
          </div>
        </div>
        <div class="mt-3">
          <div class="alert alert-info" role="alert">
            Paste the public key generated by Apex into the area below.
            The device will be created with name 'user' and email 'user@host',
            extracted from the comment part of the key.
          </div>
          <form>
            <div class="form-group">
              <label for="addDevice-pubkey">Device public key:</label>
              <textarea id="addDevice-pubkey" class="form-control"
                        rows="5" placeholder="ssh-rsa AAAAB3... user@host">
              </textarea>
            </div>
          </form>
          <div class="row">
            <button class="btn btn-primary submitbutton col-3 pt-3 pb-3 mr-3" type="button" onclick="addDevice()">
              Submit
            </button>
            <button class="btn btn-outline-secondary submitbutton col-3 pt-3 pb-3" type="button" onclick="showOverview()">
              Cancel
            </button>
          </div>
          <div id="addDevice-errors"
               class="alert alert-danger mt-3" role="alert" style="display: none;">
          </div>
          <div id="addDevice-success"
               class="alert alert-success mt-3" role="alert" style="display: none;">
            <h5>Device already exists or created</h5>
          </div>
          <div id="addDevice-output"
               class="alert alert-dark mt-3" role="alert" style="display: none;">
          </div>

          <table id="devices-list" class="table mt-3">
            <thead>
              <tr>
                <th>Existing devices:</th>
              </tr>
            </thead>
            <tbody>
            </tbody>
          </table>
        </div>
      </div>
    </div>

    <div id="linkDeviceToStudy">
      <div class="container pt-3">
        <div class="row">
          <button onclick="showOverview()" class="btn btn-outline-info col-2">
            &larr;
          </button>
          <div class="col-10 text-center">
            <h4>Linking</h4>
          </div>
        </div>
        <div class="mt-3">
          <form>
            <div class="form-group">
              <label for="linkDeviceToStudy-devices">Choose device to connect:</label>
              <select multiple id="linkDeviceToStudy-devices" class="form-control" size="20">
              </select>
            </div>
            <div class="form-group">
              <button id="linkDeviceToStudy-filterButton" type="button"
                      class="btn btn-light" onclick="linkDeviceToStudyShowPersonalDevices()">
                Personal devices
              </button>
              <button id="linkDeviceToStudy-filterButton" type="button"
                      class="btn btn-light" onclick="linkDeviceToStudyShowRemainingDevices()">
                All devices
              </button>
            </div>
            <div class="form-group">
              <label for="linkDeviceToStudy-studies">
                Choose study to connect to:
              </label>
              <select id="linkDeviceToStudy-studies" class="form-control"
                      onchange="linkDeviceToStudyFillExperimentBranches()">
              </select>
            </div>
            <div class="form-group">
              <label for="linkDeviceToStudy-branches">
                Choose experiment branches to connect to:
              </label>
              <select multiple id="linkDeviceToStudy-branches" class="form-control" size="5">
              </select>
            </div>
          </form>
          <button class="btn btn-primary submitbutton" type="button" onclick="connectDeviceToStudy()">
            Link
          </button>
          <button class="btn btn-outline-secondary submitbutton" type="button" onclick="showOverview()">
            Cancel
          </button>
          <div id="connectDeviceToStudy-errors"
               class="alert alert-danger mt-3" class="alert" style="display: none;">
          </div>
          <div id="connectDeviceToStudy-success"
               class="alert alert-success mt-3" class="alert" style="display: none;">
            <h5>Linked device(s) to study</h5>
          </div>
          <div id="connectDeviceToStudy-output"
               class="alert alert-dark mt-3" role="alert" style="display: none;">
          </div>
        </div>
      </div>
    </div>
  </body>
</html>
