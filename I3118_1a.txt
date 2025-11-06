echo "ENTER ADDRESS BOOK NAME:    "
read fname
touch $fname
echo -e "ADDRESS BOOK CREATED\n"
echo -e "NAME\t\t ID \t DOB\t\tADDRESS\t MOB_NO\t\t SALARY">>$fname
ch=0
while [ $ch -lt '7' ]
do
echo -e "ADDRESS BOOK :\n"
echo -e "1.CREATE ADDRESS BOOK"
 echo -e "2.VIEW ADDRESS BOOK"
echo -e "3.INSERT A RECORD"
echo -e "4.DELETE A RECORD"
echo -e "5.MODIFY A RECORD"
echo -e "6.SEARCH A RECORD"
echo -e "7.EXIT FROM ADDRESS BOOK"
echo -e "ENTER YOUR CHOICE"
read ch
case $ch in
	1)echo -e "enter number of records you want to enter \t"
	  read n
	  for((i=0;i<$n;i++))
	  do
	  	echo -e "ENTER NAME OF EMPLOYEE\t"
	  	read ename
	  	echo -e "ENTER ID OF EMPLOYEE\t"
	  	read eid
	  	echo -e "ENTER DOB OF EMPLOYEE\t"
	  	read edob
	  	echo -e "ENTER ADDRESS OF EMPLOYEE\t"
	  	read eadd
	  	echo -e "ENTER MOBILE NO. OF EMPLOYEE\t"
	  	read emob
	  	echo -e "ENTER SALARY OF EMPLOYEE\t"
	  	read esal
	  	echo -e " $ename \t $eid \t $edob \t $eadd \t $emob \t $esal \n">>$fname
	  done
	  ;;
	  2)
	  	cat $fname
	  ;;
	  3)
	  	echo -e "enter new record"
	  	echo -e "ENTER NAME OF EMPLOYEE\t"
	  	read ename
	  	echo -e "ENTER ID OF EMPLOYEE\t"
	  	read eid
	  	echo -e "ENTER DOB OF EMPLOYEE\t"
	  	read edob
	  	echo -e "ENTER ADDRESS OF EMPLOYEE\t"
	  	read eadd
	  	echo -e "ENTER MOBILE NO. OF EMPLOYEE\t"
	  	read emob
	  	echo -e "ENTER SALARY OF EMPLOYEE\t"
	  	read esal
	  	echo -e " $ename \t $eid \t $edob\t\t $eadd \t $emob \t $esal \n">>$fname
	  ;;
	  4)
	  	echo -e "ENTER EMPLOYEE ID TO BE DELETED "
	  	read eid
	  	if  grep -w $eid $fname 
	  	then	
	  			grep -wv $eid $fname >>temp
	  			rm $fname
	  			mv temp $fname
	  			echo "RECORD DELETED"

	  	else
	  		echo "RECORD DOES NOT EXIST "
	  	fi
	  ;;
	  5)
	  	echo "ENTER EMPLOYE  ID TO BE MODIFIY"
	  	read eid
	  	if  grep -w $eid $fname 
	  		then	
	  			grep -wv $eid $fname >>temp
	  			rm $fname
	  			mv temp $fname
	  			echo -e "enter modified record"
			  	echo -e "ENTER NAME OF EMPLOYEE\t"
	  			read ename
	  			echo -e "ENTER ID OF EMPLOYEE\t"
	  			read eid
	  			echo -e "ENTER DOB OF EMPLOYEE\t"
	  			read edob
	  			echo -e "ENTER ADDRESS OF EMPLOYEE\t"
	  			read eadd
	  			echo -e "ENTER MOBILE NO. OF EMPLOYEE\t"
	  			read emob
	  			echo -e "ENTER SALARY OF EMPLOYEE\t"
	  			read esal
	  			echo -e " $ename \t $eid \t $edob\t\t $eadd \t $emob \t $esal \n">>$fname
	  		else
	  			echo "RECORD DOES NOT EXIST "
	  	fi
	  ;;
	  6)
	  	echo -e "ENTER EMPLOYEE ID TO BE SEARCHED "
	  	read eid
	  	if  grep  $eid $fname 
	  	then
	  		grep -w $eid $fname 	
	  		echo "RECORD FOUND...!!!"
	  	else
	  		echo "RECORD DOES NOT EXIST "
	  	fi
	  ;;
	  esac
done
