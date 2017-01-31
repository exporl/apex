function [mat,labels]=confusionmatrix(a,b,addlabels)
% a=stimuli
% b=answers
% addlabels=extra labels, will be added to the list of labels
% both numeric or cell

if (nargin<3)
    if (iscell(a))
        addlabels=cell(0,0);
    else
        addlabels=[];
    end
end

if (size(a,2)>size(a,1))
    a=a';
end

if (size(b,2)>size(b,1))
    b=b';
end

if (size(addlabels,2)>size(addlabels,1))
    addlabels=addlabels';
end



if (iscell(a) && iscell(b))
    labels=unique([a; b; addlabels]);

    if (size(labels,1)>1)
        labels=labels';
    end


    mat=zeros(length(labels));
    for i=1:length(a)
        [t,la]=ismember(labels,a(i));
        Ia=find(la);
        [t,la]=ismember(labels,b(i));
        Ib=find(la);
        mat(Ia,Ib)=mat(Ia,Ib)+1;
    end


    if (sum(sum(mat))~=length(a))
        error('Internal error');
    end

elseif (isnumeric(a) && isnumeric(b))


    labels=unique([a; b; addlabels]);

    if (size(labels,1)>1)
        labels=labels';
    end


    mat=zeros(length(labels));
    for i=1:length(a)
        Ia=find(labels==a(i));
        Ib=find(labels==b(i));
        mat(Ia,Ib)=mat(Ia,Ib)+1;
    end


    if (sum(sum(mat))~=length(a))
        error('Internal error');
    end

else
    error('Datatypes do not match');

end


function alabels=getlabels(a)
s=sort(a);
alabels=[s(1)];
for i=1:length(s)
    if (s(i)~=alabels(end))
        alabels=[alabels s(i)];
    end
end
