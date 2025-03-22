

ID = 0;
Cnt = 0;
Bye = 0;
ByeCnt = 0;

function plustwo (x)
local a;
a = 2 
return x+a;
end

function set_uid(x)
ID = x;
end

function ResetBye()
Bye  = 0
end

function API_SendMessage(id, user_id, mess)
return API_SendMessage(id, user_id, mess)
end

function API_get_x(id)
return API_get_x(id)
end

function API_get_y(id)
return API_get_y(id)
end

function API_CallMove(id, user_id)
return API_CallMove(id, user_id)
end

function API_MoveEnd(id, user_id)
return API_MoveEnd(id, user_id)
end


function MoveToEnemy(target_id)
-- if문을 사용하여 a와 b의 값을 비교합니다.

    if (Cnt > 3) then
        Cnt = 0;
        API_SendMessage(ID, target_id, "Bye")
        API_MoveEnd(ID, target_id)
        return
     end
     if(0 == Cnt) then
        API_SendMessage(ID, target_id, "hello")
      end
  API_CallMove(ID, target_id)
  Cnt = Cnt  + 1;
end