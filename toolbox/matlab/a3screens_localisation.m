function result=a3screens_localisation(angles,feedbacklen,buttonid,texts,showfeedback,arclayout,ledfeedback,showcurrent)
% Create screen containing arclayout for localisation experiment
% angles: the angles to be presented, in degrees
% feedbacklen: length of feedback, if 0 no feedback
% buttonid: sprintf pattern for generating the button id's
% texts: text to put on the buttons
% arclayout: whether to use a circular layout, if 0 a grid will be used

if (nargin<3)
    buttonid='button%d';
end
if (nargin<4)
    texts=angles;
end
if (nargin<5)
    showfeedback=feedbacklen~=0;
end
if (nargin<6)
    arclayout=1;
end
if (nargin<7)
    ledfeedback=0;
end
if (nargin<8)
    showcurrent=0;
end


angles=angles;

% find angles stepsize
stepsize=abs(min(diff(abs(angles))));

% deterine arc type
if (arclayout)
    I=find(angles>90);
    if (~isempty(I))
        arctype='full';
        arcsize=360/stepsize;
        add=90;
        fullarc=1;
    else
        arctype='upper';
        arcsize=180/stepsize+1;
        add=0;
        fullarc=0;
    end
end

screens=['<general>' lf ...
            '<repeatbutton>true</repeatbutton>' lf  ...
            '<statuspicture>true</statuspicture>' lf ...
         '</general>' lf ];

screens=[screens a3reinforcement(1, feedbacklen, showcurrent,showfeedback,ledfeedback)];

answers=cell(length(angles),1);
buttons='';
buttongroup=['<buttongroup id="buttongroup">' lf];
count=1;
for dir=angles
    id=sprintf(buttonid,dir);
    answers{count}=id;

    if (arclayout)

        if (fullarc)
            where=(dir+360+add)/stepsize;
            where=mod(where,arcsize);
        else
            if (dir==90)
                where=arcsize-1;
            else
                where=mod(dir+360+90,180);
                where=where/stepsize;
            end
        end
        col=where+1;
        row=1;
    else
        row=ceil((count-0.99)/4);
        col=mod(count-1,4)+1;
    end

    if (texts(count)<10)
        shortcut = num2str(texts(count));
    else
        shortcut = '';
    end
    buttons=[buttons a3button( id, col, row, num2str(texts(count)), shortcut) ];
    buttongroup=[buttongroup '<button id="' id '"/>' lf];
    count=count+1;
end
buttongroup=[buttongroup lf '</buttongroup>' lf];

if (arclayout)
    screen=[a3arclayout(arcsize, buttons,arctype) ];
else
    screen=[a3gridlayout(4,4, buttons) ];
end
screen=[screen buttongroup lf '<default_answer_element>buttongroup</default_answer_element>' ];
result=[screens '<screen id=''screen''>' lf screen lf '</screen>'];
result=wraptag('screens',result);
