function varargout = a3result_analysis(varargin)
% A3RESULT_ANALYSIS M-file for a3result_analysis.fig
%      A3RESULT_ANALYSIS, by itself, creates a new A3RESULT_ANALYSIS or raises the existing
%      singleton*.
%
%      H = A3RESULT_ANALYSIS returns the handle to a new A3RESULT_ANALYSIS or the handle to
%      the existing singleton*.
%
%      A3RESULT_ANALYSIS('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in A3RESULT_ANALYSIS.M with the given input arguments.
%
%      A3RESULT_ANALYSIS('Property','Value',...) creates a new A3RESULT_ANALYSIS or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before a3result_analysis_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to a3result_analysis_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Copyright 2002-2003 The MathWorks, Inc.

% Edit the above text to modify the response to help a3result_analysis

% Last Modified by GUIDE v2.5 12-Mar-2007 14:48:40

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
    'gui_Singleton',  gui_Singleton, ...
    'gui_OpeningFcn', @a3result_analysis_OpeningFcn, ...
    'gui_OutputFcn',  @a3result_analysis_OutputFcn, ...
    'gui_LayoutFcn',  [] , ...
    'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before a3result_analysis is made visible.
function a3result_analysis_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to a3result_analysis (see VARARGIN)

% Choose default command line output for a3result_analysis
handles.output = hObject;

handles=setParams(handles);
handles.dir=handles.defaultpath;
handles=reloadAll(handles);

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes a3result_analysis wait for user response (see UIRESUME)
% uiwait(handles.resultanalysisFig);


% --- Outputs from this function are returned to the command line.
function varargout = a3result_analysis_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on selection change in experimentList.
function experimentList_Callback(hObject, eventdata, handles)
% hObject    handle to experimentList (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns experimentList contents as cell array
%        contents{get(hObject,'Value')} returns selected item from experimentList

%% trick to catch double clicks:
global First_Time_Execution
First_Time_Execution = 1;
pause(0.10); %approx. time for double click
% detect MouseEvent = DoubleClick;
Sel_Type = get( handles.resultanalysisFig, 'SelectionType' );
MouseEvent_IsDoubleClick = strcmp( Sel_Type, 'open' );

contents = get(hObject,'String');
selected=contents{get(hObject,'Value')};

% discard second_time_call
if First_Time_Execution
    % is double click
    if MouseEvent_IsDoubleClick
        First_Time_Execution = 0;
        handles.dir=[handles.dir delim selected ];
        reloadAll(handles);
    else
        reloadResultList(handles, selected);
    end

end




% if (strcmp(get(handles.,'SelectionType'), 'Open'))    % double click
%
% else
%
% end

% --- Executes during object creation, after setting all properties.
function experimentList_CreateFcn(hObject, eventdata, handles)
% hObject    handle to experimentList (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in resultsList.
function resultsList_Callback(hObject, eventdata, handles)
% hObject    handle to resultsList (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns resultsList contents as cell array
%        contents{get(hObject,'Value')} returns selected item from resultsList
contents = get(hObject,'String');
selected=contents{get(hObject,'Value')};
handles=loadAnalyze(handles, selected);

disp(['''' handles.fullfilename '''']);



% --- Executes during object creation, after setting all properties.
function resultsList_CreateFcn(hObject, eventdata, handles)
% hObject    handle to resultsList (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in typeCombo.
function typeCombo_Callback(hObject, eventdata, handles)
% hObject    handle to typeCombo (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns typeCombo contents as cell array
%        contents{get(hObject,'Value')} returns selected item from typeCombo


% --- Executes during object creation, after setting all properties.
function typeCombo_CreateFcn(hObject, eventdata, handles)
% hObject    handle to typeCombo (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on button press in analyzeButton.
function analyzeButton_Callback(hObject, eventdata, handles)
% hObject    handle to analyzeButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
doAnalyze(handles);


% --- Executes on button press in basePathButton.
function basePathButton_Callback(hObject, eventdata, handles)
% hObject    handle to basePathButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

handles.dir=uigetdir(handles.defaultpath);
if (~length(handles.dir))
    return;
end

guidata(hObject, handles);
reloadAll(handles);


function handles=setParams(handles)
handles.defaultpath=basestimpath;

handles.TYPE_LGF=1;
handles.TYPE_PITCHMATCH=2;
handles.TYPE_ILDARC=3;
handles.TYPE_ILDJND=4;
handles.TYPE_LOC=5;
handles.TYPE_ITD=6;
handles.TYPE_THRESH=7;
handles.TYPE_CUSTOM=8;

function handles=reloadAll(handles)
dir_struct = dir(handles.dir);
[sorted_names,sorted_index] = sortrows({dir_struct.name}');

isdirs=zeros(length(dir_struct),1);
for c=1:length(dir_struct)
    isdirs(c)=dir_struct(c).isdir;
    if (strcmp(dir_struct(c).name,'.'))
        isdirs(c)=0;
    end
end
handles.file_names = sorted_names(find(isdirs));


set(handles.experimentList,'String',handles.file_names,...
    'Value',1);
set(handles.resultsList, 'String', []);

guidata(handles.resultanalysisFig,handles);


function handles=hideStuff(handles)
set(handles.lgfParameters, 'Visible', 'off');
set(handles.locparams, 'Visible', 'off');
set(handles.rateText, 'String', '');
set(handles.analyzeButton, 'Enable', 'off');


function handles=reloadResultList(handles,subdir)
dir_struct = dir([handles.dir delim subdir]);

hideStuff(handles);
xmlfound=0;
for c=1:length(dir_struct)
    filename=dir_struct(c).name;
    if (dir_struct(c).isdir)
        pat='rate_(\d)';
        tags=regexp(filename, pat, 'tokens');
        if (length(tags))
            fullpath=[handles.dir delim subdir delim filename];
            handles=loadResultList(handles, fullpath);
            set(handles.rateText, 'String', filename);
            break;
        end
    else
        if (strcmp(getext(filename), 'xml') || strcmp(getext(filename), 'apr'))
            xmlfound=1;
            fullpath= [handles.dir delim subdir];
            handles=loadResultList(handles, fullpath);
            break;
        end
    end
end




% populate results list according to the full path
function handles=loadResultList(handles,fullpath)
dir_struct = cat(1, ...
    dir([fullpath delim '*.xml']), ...
    dir([fullpath delim '*.apr']));

items=cell(0,0);
tags={};
for c=1:length(dir_struct)
    filename=dir_struct(c).name;


    if (length(filename)>3)
        if (strcmp(filename(end-2:end), 'apr'))
            items=cat(1,items, {filename});
        else

            %pat='^(.*)-results-{0,1}(\d*)\.xml';
            pat='^(.*)-results.*\.(xml|apr)';
            tags=regexp(filename, pat, 'tokens');
            if (length(tags))
                items=cat(1,items, {filename});
            end
        end
    end

end

handles.resultfiles=items;
if (length(tags))
    handles.type=tags{1}{1};
end
handles.basepath=fullpath;

set(handles.resultsList,'String',handles.resultfiles,...
    'Value',1);

if (length(handles.resultfiles))     % select the first one
    handles=loadAnalyze(handles, handles.resultfiles{1});
    set(handles.resultsList, 'Value', 1);
end


guidata(handles.resultanalysisFig,handles);



% --- Executes on button press in reloadButton.
function reloadButton_Callback(hObject, eventdata, handles)
% hObject    handle to reloadButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
reloadAll(handles);


function ext=getext(filename)
if (length(filename)>=4 && filename(end-3)=='.')
    ext=filename(end-2:end);
else
    ext='';
end


% show analyze part of screen for certain file
function handles=loadAnalyze(handles, filename);

%% get file type
%pat='^(.*)-results-{0,1}(\d*).(xml|apx)';
pat='^(.*)-.*\.(xml|apr)';
tags=regexp(filename, pat, 'tokens');
type=tags{1}{1};

handles.fullfilename=[handles.basepath delim filename];

hideStuff(handles);

ok=1;
[handles,parsenamesuccess]=parsescriptname(handles,type);


% try to get experment type from processing tag
[script, err]=a3getmatlabscript(handles.fullfilename);
if (~err)
    handles.scriptname=script;
end
if (err && ~parsenamesuccess)
    ok=0;
    disp(['Unknown experiment type ' type]);
end



if (ok)
    set(handles.analyzeButton, 'Enable', 'on');
end

guidata(handles.resultanalysisFig, handles);


function [handles,success]=parsescriptname(handles,type)
success=1;


%     [script,success]=a3getmatlabscript(handles.fullfilename);
%     success=~success;
%     if (success)
%             handles.scriptname=script;
%             handles.type=handles.TYPE_CUSTOM;
if (strcmp(type,'lgf'))
    set(handles.lgfParameters, 'Visible', 'on');
    handles.type=handles.TYPE_LGF;
% elseif (strcmp(type,'pitchmatch'))
%     handles.type=handles.TYPE_PITCHMATCH;
elseif (strncmp(type,'boost',5))
    handles.type=handles.TYPE_ILDARC;
elseif (strcmp(type,'a3ildjnd'))
    handles.type=handles.TYPE_ILDJND;
elseif (strcmp(type,'a3locresults'))
    set(handles.locparams, 'Visible', 'on');
    handles.type=handles.TYPE_LOC;
    %    elseif (strncmp(type,'itd',3))
    %        handles.type=handles.TYPE_ITD;
% elseif (strncmp(type,'thresh',6))
%     handles.type=handles.TYPE_THRESH;
else
    handles.type=handles.TYPE_CUSTOM;
    success=0;
end

%     if ( strcmp(handles.scriptname,'a3locresults'))
%         set(handles.locparams, 'Visible', 'on');
%          handles.type=handles.TYPE_LOC;
%     end


function handles=doAnalyze(handles)

disp(['Analyzing ' handles.fullfilename]);

if (handles.type==handles.TYPE_LGF)
    doAnalyzeLGF(handles);
elseif (handles.type==handles.TYPE_PITCHMATCH)
    doAnalyzePitchmatch(handles);
elseif (handles.type==handles.TYPE_ILDARC)
    a3locresults(handles.fullfilename);
elseif (handles.type==handles.TYPE_ILDJND)
    a3ild_jnd_results(handles.fullfilename);
elseif (handles.type==handles.TYPE_LOC)
    doAnalyzeLoc(handles);
    %elseif (handles.type==handles.TYPE_ITD)
    %    a3itd_afc_results(handles.fullfilename);
elseif (handles.type==handles.TYPE_THRESH)
    a3thresh_results(handles.fullfilename);
elseif (handles.type==handles.TYPE_CUSTOM)
    disp(['Executing script: ' handles.scriptname]);
    eval( [char(handles.scriptname) '(''' handles.fullfilename ''')']);
else
    disp('doAnalyze: Internal error');
end




function handles=doAnalyzeLGF(handles)
% get sort by
get(handles.lgfSortBy, 'SelectedObject')
if (get(handles.eRadio, 'Value'))
    sortby='e';
else
    sortby='a';
end

sepfig=get(handles.lgfSepFig, 'Value');

a3lgf_results(handles.fullfilename, sepfig, sortby);


function handles=doAnalyzeLoc(handles)
% get sort by
get(handles.verbosity, 'SelectedObject')
if (get(handles.verb1Radio, 'Value'))
    verbosity=1;
else
    verbosity=2;
end



a3locresults(handles.fullfilename, '', verbosity);


function handles=doAnalyzePitchmatch(handles)
figure;
a3pitchmatch_results(handles.fullfilename);



% --- Executes on button press in lgfSepFig.
function lgfSepFig_Callback(hObject, eventdata, handles)
% hObject    handle to lgfSepFig (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of lgfSepFig


% --- Executes on button press in closeFigButton.
function closeFigButton_Callback(hObject, eventdata, handles)
% hObject    handle to closeFigButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
close all;


% --- Executes on button press in checkbox2.
function checkbox2_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox2
