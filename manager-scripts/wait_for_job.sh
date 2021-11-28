#!/bin/bash
SECONDS=0
job_name=$1
end=$2
echo "Wait for end (Failed or Complete) of default namespace job $job_name for $end seconds."

until [[ $SECONDS -gt $end ]] || [[ $(kubectl get jobs -n default $job_name -o jsonpath='{.status.conditions[?(@.type=="Failed")].status}') == "True" ]] || [[ $(kubectl get jobs -n default $job_name -o jsonpath='{.status.conditions[?(@.type=="Complete")].status}') == "True" ]]; do 
	sleep 1
	SECONDS=$(($SECONDS+1)) 
done
echo ""
if [[ $(kubectl get jobs -n default $job_name -o jsonpath='{.status.conditions[?(@.type=="Complete")].status}') == "True" ]]; then
	echo "Job finished"
	exit 0
fi

if [[ $(kubectl get jobs -n default $job_name -o jsonpath='{.status.conditions[?(@.type=="Failed")].status}') == "True" ]]; then
	echo "Failed"
	exit 1
fi

echo "Timeout"
exit 2
