


## Commands

* Edit crontab jobs

	`crontab -e`

* Activate 
	
	`/etc/init.d/cron enable`
	
* Show jobs

	`crontab -l`


## Format

min (0-59) 
hour (0-23)
day/month (1-31)
month (1-12)
day/week (0-6). Note: 0 is Sunday
command

\*:  every
\*/3: every 3
1,3:  1 and 3

