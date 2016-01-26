function [Dnodes,Inodes,Iner,VInx,VUnx]=FindDInodes(topo)
%Function [Dnodes,Inodes,Iner,VInx,VUnx]=FindDInodes(Topology)
%Find dependent, independent, VS and inercial nodes
%IN:
%Topology - shema topology matrix
%OUT:
%Dnodes - depended nodes numbers list
%Inodes - independed nodes numbers list
%Iner - inercial nodes matrix
%VInx - thyristor current node indexes
%VUnx - thyristor voltage node indexes
%Author: Bilokha D.O.

Dnodes=[]; Inodes=[];
Iner=[]; VInx=[];
VUnx=[];
for k=1:size(topo,1)
    switch topo(k,1)
    case 1
        Inodes(length(Inodes)+1)=3*k;
        if topo(k,6)~=0
            Dnodes(length(Dnodes)+1)=3*k-1;
        end
    case 7
        Inodes(length(Inodes)+1)=3*k-1;
        if topo(k,6)~=0
            Dnodes(length(Dnodes)+1)=3*k;
        end
    case 3
        Dnodes(length(Dnodes)+1)=3*k;
        VUnx(length(VUnx)+1)=length(Dnodes);
        Dnodes(length(Dnodes)+1)=3*k-1;
        VInx(length(VInx)+1)=length(Dnodes);
    case 5
        Dnodes(length(Dnodes)+1)=3*k;
        VUnx(length(VUnx)+1)=length(Dnodes);
        Dnodes(length(Dnodes)+1)=3*k-1;
        VInx(length(VInx)+1)=length(Dnodes);
    case 2
        Dnodes(length(Dnodes)+1)=3*k;
        Iner(size(Iner,1)+1,:)=[3*k-2 length(Dnodes)];
        if topo(k,6)~=0
            Dnodes(length(Dnodes)+1)=3*k-1;
        end
    case 6
        Dnodes(length(Dnodes)+1)=3*k-1;
        Iner(size(Iner,1)+1,:)=[3*k-2 length(Dnodes)];
        if topo(k,6)~=0
            Dnodes(length(Dnodes)+1)=3*k;
        end
    otherwise
        if (topo(k,6)==1)|(topo(k,6)==3)
            Dnodes(length(Dnodes)+1)=3*k-1;
        end
        if topo(k,6)==2|(topo(k,6)==3)
            Dnodes(length(Dnodes)+1)=3*k;
        end
    end
end
Inodes=Inodes';