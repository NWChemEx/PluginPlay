Simulation Development Environment
==================================

Getting Started with the SDE
----------------------------

Topics in this section are targeted at users who want to run the SDE and 
possibly dynamically influence the control flow.

- [Terminology](dox/terminology.md) details what SDE specific terms mean
- [Overview](dox/overview.md) provides a high-level overview of the SDE
- [User Workflow](dox/workflow.md) details a typical workflow
- [App Workflow](dox/AppFlow.md) details the basics of running an app as well as
  how to write simple "lambda apps"
- [Common Tasks](dox/CommonTasks.md) explicitly details how to perform common 
  computational chemistry tasks within an app  

Extending the SDE
-----------------

Topics in this section are meant for users who want to write apps and/or make 
their libraries discoverable/usable with the SDE, but don't care about the 
internals of the SDE more than they need to. 

- [Advanced App Workflow](dox/AdvancedAppFlow.md) details the app lifecycle in
  much greater detail to ensure users can get the most out of their apps
- [AppInfo API](dox/AppInfoAPI.md) details about the available meta-data options
  for an app  
- [Setting Parameters](dox/parameters.md) details the types of parameters 
  available and the features provided by the Parameters class
  
Understanding the SDE
---------------------

Topics in this section are meant for users who want to understand how the SDE
works in more detail.

- [App API](dox/APPAPIConsiderations.md) in depth discussion of how the API came
  to be
- [Memoization Considerations](dox/memoization.md) discusses the fine details
  of memoization  
- [Running an App](dox/RunningAnApp.md) details what happens when a user 
  calls `sde.run`
- [Cache Considerations](dox/cache.md) everything you wanted to know about the 
  cache (and more) 
  
  
