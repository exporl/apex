function result=a3locresults(filenames,ntitle,verbosity,p)
% a3locresults(filename)
%
% verbosity:
%   1   normal
%   2   show roving correlations

if (nargin<2)
    ntitle='';
end
if (nargin<3)
    verbosity=1;
end
if (nargin<4)
    p=struct;
end
p=ef(p,'subplot', 0);
p=ef(p,'plot_resolved', 0);                 % plot resolved confusion matrix
p=ef(p,'plot_rovecorr', 0);                 % plot correlation rove/...
p=ef(p,'no_plot', 0);                       % do not plot anything
p=ef(p,'markersize',20);                    % size of dots in confusion matrix
p=ef(p,'roveplot_binsize',5);               % binsize of the rove error plot
%p=ef(p,'select_angles', []);                % use only stimuli from the given angles in analysis
set(0,'defaulttextinterpreter','none');     % otherwise slooooow


% read transformed results from file
if (~iscell(filenames))
    filenames={filenames};
end

buttons=[];
stimuli=[];
roves=[];

for f=1:length(filenames)
    filename=filenames{f};
    if (isempty(filename))
        warning('Skipping empty filename');
        continue;
    end

    results=a3getresults(filename);


    shortfilename=filename;
    pos=0;
    for i=1:length(filename)
        if (filename(i) == '/')
            pos=i;
        end
    end
    if (pos)
        shortfilename=shortfilename(pos+1:end);
    end

    parseok=0;

    I=strmatch('Header=trial;stimulus;correctanswer;corrector;useranswer', results);
    if (~isempty(I))            %% new format
        s=a3parseresults(results);

        thisstimuli=zeros(length(s),1);
        thisbuttons=thisstimuli;
        thisroves=thisstimuli;
        for i=1:length(s)
            thisstimuli(i)=getstimulusdegrees(s(i).stimulus);
            thisbuttons(i)=getbuttondegrees(s(i).useranswer);
            if (isfield(s, 'roving'))
                thisroves(i)=str2double(s(i).roving);
            else
                thisroves(i) = getstimulusrove(s(i).stimulus);
            end
        end

        parseok=1;
    end


    % look for the stimulus line
    I=strmatch('stimulus', results);
    if (~parseok && ~isempty(I))

        thisstimuli=sscanf(results{I}, 'stimulus_%fDEG\t');
        if (length(thisstimuli)==0)
            thisstimuli=sscanf(results{I}, 'stimulus%f\t');
        end
        eas=0;
        if (length(thisstimuli)==1)     % eas
            thisstimuli=sscanf(results{I}, 'stimulus_%fDEG_rove%*d\t');
            thisroves=sscanf(results{I}, 'stimulus_%*fDEG_rove%d\t');
            eas=1;
        end



        % look for the response line
        I=strmatch('button', results);
        if (isempty(I))
            error('Button line not found');
        end

        thisbuttons=sscanf(results{I}, 'button%f\t');



        if (length(thisstimuli)~=length(thisbuttons))
            error('Lengths don''t match');
        end

        if (~eas)
            thisroves=sscanf(results{I+2}, '%f\t');
        end




        parseok=1;
    end

    if (~parseok)
        error(['Could not parse results from XSLT script for file ' filename]);
    end

    stimuli=[stimuli; thisstimuli];
    roves=[roves; thisroves];
    buttons=[buttons; thisbuttons];


end     % filenames

if (length(filenames)>1)
    shortfilename='*** mult files ***';
end
if (length(ntitle))
    shortfilename=[shortfilename ' - ' ntitle];
end

if (verbosity)
    disp(shortfilename);
end


% build confusion matrix
[matrix,slabels]=confusionmatrix(stimuli,buttons);
% matrix(x,y): x=stimuli, y=buttons


% if (~isempty(p.select_angles))
%     labelkeep=[];        % indices in slabels that can stay
%     for i=1:length(slabels)
%         if (find(p.select_angles==slabels(i)))
%             labelkeep=[labelkeep i];
%         end
%     end
%
%     slabels_select=slabels(labelkeep);
%     matrix=matrix(labelkeep,:);
% else
slabels_select=slabels;
% end

%matrix_select=matrix;       % REMOVEME


%% calculate error measures

rms=rmserr(slabels,matrix);
lsf=lsfit(stimuli,buttons);


%% Calculate number of front-back reversals
matrix_resolved=zeros(size(matrix));
nreversals=0;
for row=1:size(matrix,1)
    for col=1:size(matrix,2)
        if (matrix(row,col))

            if (abs(slabels_select(row))<=90 && abs(slabels(col))>90)
                % move answer to front
                %                 continue;     %% FIXME!!
                target=sign_no0(slabels(col))*(180-abs(slabels(col)));
                I=find(slabels==target);
                if (isempty(I))
                    error('Cannot find label');
                end
                if (I==col)
                    error('Cannot move');
                end
                matrix_resolved(row,I)=matrix_resolved(row,I)+matrix(row,col);
                nreversals=nreversals+matrix(row,col);
            elseif (abs(slabels_select(row))>90 && abs(slabels(col))<90)
                % move answer to back
                %                 continue;           %% FIXME!!
                target=sign_no0(slabels(col))*(180-abs(slabels(col)));
                I=find(slabels==target);
                if (isempty(I))
                    error('Cannot find label');
                end
                if (I==col)
                    error('Cannot move');
                end
                matrix_resolved(row,I)=matrix_resolved(row,I)+matrix(row,col);
                nreversals=nreversals+matrix(row,col);
            else
                matrix_resolved(row,col)=matrix_resolved(row,col)+matrix(row,col);
                continue;
            end
        end
    end
end

if (sum(matrix(:))~=sum(matrix_resolved(:)))
    error('Internal error');
end

nfrontback=nreversals/sum(sum(matrix))*100;
if (verbosity)
    disp(['Number of front-back confusions: '  num2str(nfrontback)] );
end

rms_resolved=rmserr(slabels,matrix_resolved);

%% Calculate correlation rove/answer
if (~sum(isnan(roves)) && sum(abs(diff(roves))))
    ft_ = fittype('poly1' );
    [frove,grove] = fit(roves,buttons,ft_ );
    result.rovecorr.f=frove;
    result.rovecorr.g=grove;
end

%% Calculate relation rove/error

result.rovelabels=[ quantize(min(roves), p.roveplot_binsize):p.roveplot_binsize:quantize(max(roves), p.roveplot_binsize)];
rresults=zeros(size(result.rovelabels,1),size(result.rovelabels,2));
for irl=1:length(result.rovelabels)
    I=find(quantize(roves,p.roveplot_binsize)==result.rovelabels(irl));
    %I=quantize(rovelabels(ilr), p.roveplot_binsize);
    rresults(irl)=mean(abs(stimuli(I)-buttons(I)));
end
result.error_per_rovelabel=rresults;

%% PLOT %%

if (p.plot_rovecorr)
    % plot correlation roves/answers
    figure
    plot(roves, buttons,'.');
    hold on
    plot(frove)
    xlabel('rove (dB)');
    ylabel('button (degrees)');
    title(['Correlation rove/response - ' shortfilename]);


    figure
    plot(result.rovelabels,rresults,'-*');
    xlabel('Rove');
    ylabel('mean absolute error');
    title(['Correlation rove/error - ' shortfilename]);
end


if (p.plot_resolved)
    numfigs=3;
else
    numfigs=2;
end


if (~p.no_plot)
    %% Plot average response per stimulus
    figure
    if (p.subplot)
        subplot(1,numfigs,1);
    end
    hold on
    errorbar(slabels_select', [rms.mean_angle], [rms.std_angle],'b')
    xlabel('stimulus (degrees)');
    ylabel('response (degrees)');
    ylim([min(slabels_select)-10 max(slabels_select)+10]);
    grid on;
    xlim(get(gca,'xlim'));
    set(gca,'XTick',[min(slabels_select):30:max(slabels_select)]);
    set(gca,'YTick',[min(slabels_select):30:max(slabels_select)]);


    % regression line
    plot( [-360 360], lsf.b+lsf.a*[-360 360], 'r--');
    % Perfect score
    plot( [-360 360], [-360 360], 'k--');


    if (nfrontback)
        t{1}=['rms total R=' sprintf('%2.2f', rms_resolved.rms_total)] ;
        t{2}=['total err R (bias)=' sprintf('%2.2f', rms_resolved.total_error)];
        t{3}=['% correct R=' sprintf('%2.2f', rms_resolved.percent_correct*100)];
    else
        t{1}=sprintf('RMS=%2.2f', rms.rms_total);
        t{2}=sprintf('a*x+b with');
        t{3}=sprintf('a=%2.1f, b=%2.1f', lsf.a, lsf.b);
        t{4}=sprintf('RMS fit=%2.1f', lsf.rms);
%         t{4}=sprintf('ABS fit=%2.1f', lsf.absolute_error);
        if (sum([find(slabels_select<-90) find(slabels_select)>90]))
            t{4}=['% front-back conf=' sprintf('%2.2f', nfrontback)];
        end
    end
    if (verbosity)
        disp(t);
    end

    if (length(slabels)>4)
        lq=2;
    else
        lq=1;
    end
    text(slabels(lq),70,t);

    %% plot circle diagram
    if (p.subplot)
        subplot(1,numfigs,2)
        hold on
    else
        figure
    end
    plot_circles(slabels_select,slabels, matrix',p.markersize);
    if (~p.subplot)
        title(shortfilename);
    end
    xlabel('stimulus (degrees)');
    ylabel('response (degrees)');
    %%%%%%%%%%%
    %% plot regression line
    hold on
    plot( [-360 360], lsf.b+lsf.a*[-360 360], 'r--');

    plot( [-360 0], lsf.b_left+lsf.a_left*[-360 0], 'g--');
    plot( [0 360], lsf.b_right+lsf.a_right*[0 360], 'c--');

    if (p.subplot)
        suptitle(shortfilename);
    else
        title(shortfilename);
    end


    %%%%%%%%%%%

    if (p.plot_resolved)
        if (p.subplot)
            subplot(1,numfigs,3)
            hold on
        else
            figure
        end

        plot_circles(slabels_select,slabels,matrix_resolved',p.markersize);
        title('Confusions resolved');
    end



end


%result=struct;

names=fieldnames(rms);
for i=1:length(names)
    name=names{i};
    result=setfield(result,name, getfield(rms,name));
end
result.confusionmatrix=matrix;

result.resolved=struct;
names=fieldnames(rms_resolved);
for i=1:length(names)
    name=names{i};
    result.resolved=setfield(result.resolved,name, getfield(rms_resolved,name));
end

% result.percent_frontback=nfrontback;
% result.rms_total=rms_total;
% result.rms_total_stdev=rms_stdev;
% result.rms_angle=rms_angle;
% result.total_error=total_error;
result.angles=slabels;

% result.rms_total_resolved=rms_total_resolved;
% result.rms_total_stdev_resolved=rms_stdev_resolved;
% result.rms_angle_resolved=rms_angle_resolved;
% result.total_error_resolved=total_error_resolved;
result.filename=filenames;

result.lsf=lsf;
result.lsf.info='Least squares fit of response=a*stimulus+b';

function alabels=getlabels(a)
s=sort(a);
alabels=[s(1)];
for i=1:length(s)
    if (s(i)~=alabels(end))
        alabels=[alabels s(i)];
    end
end


function r=sign_no0(a)
if (a==0)
    r=1;
else
    r=sign(a);
end



function r=getstimulusdegrees(s)
% r=sscanf(s, 'stimulus%f\t');
names=regexp(s,'stimulus(-?\d+)+', 'tokens');
if (isempty(names))
    error(['Could not determine angle from stimulus ' s]);
end
r=str2num(names{1}{1});

function r=getstimulusrove(s)
names=regexp(s,'rove(-?\d+)+', 'tokens');
if (isempty(names))
    r=NaN;
    return;
end
r=str2num(names{1}{1});

function r=getbuttondegrees(s)
r=sscanf(s, 'button%f\t');



function s=rmserr(labels,confmat)

%labelmat=repmat(slabels, length(slabels),1);
labelmat=repmat(labels, size(confmat,1),1);

errormat=repmat(labels',1,size(confmat,2));


%disp('Square error matrix:');
%square_error=matrix.*(labelmat-labelmat').^2;
square_error=confmat.*(labelmat-errormat).^2;
s.rms_total=sqrt(sum(sum(square_error))/sum(sum(confmat)));
s.rms_stdev=sqrt(sum(sum(square_error-s.rms_total))/sum(sum(confmat)));

%disp('RMS error per angle');
%nlabels
s.rms_angle=sqrt(sum(square_error,2)./sum(confmat,2));

s.num_correct=sum(diag(confmat));
s.percent_correct=s.num_correct/sum(sum(confmat));


%disp('Total error (bias):');
%total_error_matrix=matrix.*(labelmat-labelmat');
total_error_matrix=confmat.*(labelmat-errormat);
%s.total_error=sum(sum(total_error_matrix))/sum(sum(confmat));
s.total_error=sum(sum(total_error_matrix));
s.total_error_angle=sum(total_error_matrix,2)./sum(confmat,2);

% mean response per stimulus
s.mean_angle=sum(confmat.*labelmat,2)./sum(confmat,2);
s.std_angle=sqrt(sum(confmat.* (labelmat-repmat(s.mean_angle,1,size(confmat,1))).^2 ,2)./(sum(confmat,2)-1));

absolute_error_matrix=confmat.*abs(labelmat-errormat);
s.absolute_error_angle=sum(absolute_error_matrix,2)./sum(confmat,2);
s.absolute_error=sum(sum(absolute_error_matrix))/sum(sum(confmat));



% Least squares fit of line through matrix
function r=lsfit(stimulus,response)

% entire matrix
[p,s]=polyfit(stimulus,response,1);

r.a=p(1);
r.b=p(2);
r.rms=sqrt(mean((r.a*stimulus+r.b - response).^2));
r.absolute_error=mean(abs(r.a*stimulus+r.b - response));

% left hand side
I=find(stimulus<=0);
[p,s]=polyfit(stimulus(I),response(I),1);
r.a_left=p(1);
r.b_left=p(2);

% right hand side
I=find(stimulus>=0);
[p,s]=polyfit(stimulus(I),response(I),1);
r.a_right=p(1);
r.b_right=p(2);
