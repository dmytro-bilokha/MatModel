function [CurrentEN]=MarkCycle(Branches, Con)
%Function [Cycle]=MarkCycle(Branches, Con) 
%Find all branches that form cycle with the connection
%and mark them +1 or -1
%IN:
%Branches - matrix of branches
%Con - given connection
%OUT:
%Cycle - list of branches with makrs
%Author: Bilokha D.O.

CurrentEN=[];
CurrentEN(1,1)=0;
CurrentEN(1,2:4)=Con;
CurrentEN(1,5)=0;
FindFrom=1;
while ~isempty(CurrentEN)
    found=logical(0);
    if FindFrom<=size(Branches,1)
        for k=FindFrom:size(Branches,1)
            if CurrentEN(size(CurrentEN,1),1)~=k
                if CurrentEN(size(CurrentEN,1),4)==Branches(k,2)
                    found=logical(1);
                    CurrentEN(size(CurrentEN,1)+1,1)=k;
                    CurrentEN(size(CurrentEN,1),2:4)=Branches(k,:);
                    CurrentEN(size(CurrentEN,1),5)=1;
                    break
                else
                    if CurrentEN(size(CurrentEN,1),4)==Branches(k,3)
                        found=logical(1);
                        CurrentEN(size(CurrentEN,1)+1,1)=k;
                        CurrentEN(size(CurrentEN,1),2)=Branches(k,1);
                        CurrentEN(size(CurrentEN,1),3:4)=Branches(k,3:-1:2);
                        CurrentEN(size(CurrentEN,1),5)=-1;
                        break
                    end
                end
            end
        end
    end
    if ~found
        FindFrom=CurrentEN(size(CurrentEN,1),1)+1;
        CurrentEN(size(CurrentEN,1),:)=[];
    else
        FindFrom=1;
        if CurrentEN(size(CurrentEN,1),4)==CurrentEN(1,3)
            break
        end
    end
end
CurrentEN(:,1)=[]; CurrentEN(:,2:3)=[];
CurrentEN(1,:)=[];
