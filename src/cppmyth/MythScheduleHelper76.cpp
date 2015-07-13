/*
 *      Copyright (C) 2005-2015 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

///////////////////////////////////////////////////////////////////////////////
////
//// Version helper for database up to 1309 (0.27)
////
//// Remove the Timeslot and Weekslot recording rule types. These rule
//// types are too rigid and don't work when a broadcaster shifts the
//// starting time of a program by a few minutes. Users should now use
//// Channel recording rules in place of Timeslot and Weekslot rules. To
//// approximate the old functionality, two new schedule filters have been
//// added. In addition, the new "This time" and "This day and time"
//// filters are less strict and match any program starting within 10
//// minutes of the recording rule time.
//// Restrict the use of the FindDaily? and FindWeekly? recording rule types
//// (now simply called Daily and Weekly) to search and manual recording
//// rules. These rule types are rarely needed and limiting their use to
//// the most powerful cases simplifies the user interface for the more
//// common cases. Users should now use Daily and Weekly, custom search
//// rules in place of FindDaily? and FindWeekly? rules.
//// Any existing recording rules using the no longer supported or allowed
//// types are automatically converted to the suggested alternatives.
////

#include "MythScheduleHelper76.h"
#include "../client.h"
#include "../tools.h"

#include <cstdio>
#include <cassert>

using namespace ADDON;

bool MythScheduleHelper76::FillTimerEntryWithRule(MythTimerEntry& entry, const MythRecordingRuleNode& node) const
{
  // Assign timer type regarding rule attributes. The match SHOULD be opposite to
  // that which is applied in function 'NewFromTimer'

  MythRecordingRule rule = node.GetRule();
  XBMC->Log(LOG_DEBUG, "76::%s: RecordID %u", __FUNCTION__, rule.RecordID());

  switch (rule.Type())
  {
    case Myth::RT_SingleRecord:
      {
        // Fill recording status from its upcoming
        MythScheduleList recordings = m_manager->FindUpComingByRuleId(rule.RecordID());
        MythScheduleList::const_reverse_iterator it = recordings.rbegin();
        if (it != recordings.rend())
          entry.recordingStatus = it->second->Status();
        else
          return false; // Don't transfer single without upcoming
      }
      if (rule.SearchType() == Myth::ST_ManualSearch)
        entry.timerType = TIMER_TYPE_MANUAL_SEARCH;
      else
        entry.timerType = TIMER_TYPE_THIS_SHOWING;
      entry.chanid = rule.ChannelID();
      entry.callsign = rule.Callsign();
      break;

    case Myth::RT_OneRecord:
      entry.timerType = TIMER_TYPE_RECORD_ONE;
      if (rule.Filter() & Myth::FM_ThisChannel)
      {
        entry.chanid = rule.ChannelID();
        entry.callsign = rule.Callsign();
      }
      break;

    case Myth::RT_DailyRecord:
      entry.timerType = TIMER_TYPE_RECORD_DAILY;
      if (rule.Filter() & Myth::FM_ThisChannel)
      {
        entry.chanid = rule.ChannelID();
        entry.callsign = rule.Callsign();
      }
      break;

    case Myth::RT_WeeklyRecord:
      entry.timerType = TIMER_TYPE_RECORD_WEEKLY;
      if (rule.Filter() & Myth::FM_ThisChannel)
      {
        entry.chanid = rule.ChannelID();
        entry.callsign = rule.Callsign();
      }
      break;

    case Myth::RT_AllRecord:
      if ((rule.Filter() & Myth::FM_ThisDayAndTime))
        entry.timerType = TIMER_TYPE_RECORD_WEEKLY;
      else if ((rule.Filter() & Myth::FM_ThisTime))
        entry.timerType = TIMER_TYPE_RECORD_DAILY;
      else if ((rule.Filter() & Myth::FM_ThisSeries))
        entry.timerType = TIMER_TYPE_RECORD_SERIES;
      else
        entry.timerType = TIMER_TYPE_RECORD_ALL;
      if (rule.Filter() & Myth::FM_ThisChannel)
      {
        entry.chanid = rule.ChannelID();
        entry.callsign = rule.Callsign();
      }
      break;

    case Myth::RT_OverrideRecord:
      entry.timerType = TIMER_TYPE_OVERRIDE;
      entry.chanid = rule.ChannelID();
      entry.callsign = rule.Callsign();
      break;

    case Myth::RT_DontRecord:
      entry.timerType = TIMER_TYPE_DONT_RECORD;
      entry.chanid = rule.ChannelID();
      entry.callsign = rule.Callsign();
      break;

    default:
      entry.timerType = TIMER_TYPE_UNHANDLED;
      entry.chanid = rule.ChannelID();
      entry.callsign = rule.Callsign();
      break;
  }

  switch (rule.SearchType())
  {
    case Myth::ST_TitleSearch:
      entry.epgSearch = rule.Description();
      break;
    case Myth::ST_KeywordSearch:
      entry.epgSearch = rule.Description();
      entry.timerType = TIMER_TYPE_SEARCH_KEYWORD;
      break;
    case Myth::ST_PeopleSearch:
      entry.epgSearch = rule.Description();
      entry.timerType = TIMER_TYPE_SEARCH_PEOPLE;
      break;
    case Myth::ST_PowerSearch:
      entry.epgSearch = rule.Description();
      entry.timerType = TIMER_TYPE_UNHANDLED;
      break;
    case Myth::ST_NoSearch: // EPG based
      entry.epgCheck = true;
      entry.epgSearch = rule.Title();
      break;
    case Myth::ST_ManualSearch: // Manual
      entry.chanid = rule.ChannelID();
      entry.callsign = rule.Callsign();
      entry.startTime = rule.StartTime();
      entry.endTime = rule.EndTime();
      break;
    default:
      break;
  }

  // For all repeating fix timeslot as needed
  switch (entry.timerType)
  {
    case TIMER_TYPE_RECORD_ONE:
    case TIMER_TYPE_RECORD_WEEKLY:
    case TIMER_TYPE_RECORD_DAILY:
    case TIMER_TYPE_RECORD_ALL:
    case TIMER_TYPE_RECORD_SERIES:
    case TIMER_TYPE_SEARCH_KEYWORD:
    case TIMER_TYPE_SEARCH_PEOPLE:
    case TIMER_TYPE_UNHANDLED:
      if (difftime(rule.NextRecording(), 0) > 0)
      {
        // fill timeslot starting at next recording
        entry.startTime = entry.endTime = rule.NextRecording();
        timeadd(&entry.endTime, difftime(rule.EndTime(), rule.StartTime()));
      }
      else if (difftime(rule.LastRecorded(), 0) > 0)
      {
        // fill timeslot starting at last recorded
        entry.startTime = entry.endTime = rule.LastRecorded();
        timeadd(&entry.endTime, difftime(rule.EndTime(), rule.StartTime()));
      }
      else
      {
        entry.startTime = rule.StartTime();
        entry.endTime = rule.EndTime();
      }
      break;
    default:
      entry.startTime = rule.StartTime();
      entry.endTime = rule.EndTime();
  }

  // fill others
  entry.title = rule.Title();
  entry.category = rule.Category();
  entry.startOffset = rule.StartOffset();
  entry.endOffset = rule.EndOffset();
  entry.dupMethod = rule.DuplicateControlMethod();
  entry.priority = rule.Priority();
  entry.expiration = GetRuleExpirationId(MythScheduleManager::RuleExpiration(rule.AutoExpire(), rule.MaxEpisodes(), rule.NewExpiresOldRecord()));
  entry.isInactive = rule.Inactive();
  entry.firstShowing = (rule.Filter() & Myth::FM_FirstShowing ? true : false);
  entry.recordingGroup = GetRuleRecordingGroupId(rule.RecordingGroup());
  entry.entryIndex = MythScheduleManager::MakeIndex(rule); // rule index
  if (node.IsOverrideRule())
    entry.parentIndex = MythScheduleManager::MakeIndex(node.GetMainRule());
  else
    entry.parentIndex = 0;
  return true;
}

MythRecordingRule MythScheduleHelper76::NewFromTimer(const MythTimerEntry& entry, bool withTemplate)
{
  // Create a recording rule regarding timer attributes. The match SHOULD be opposite to
  // that which is applied in function 'FillTimerEntry'

  MythRecordingRule rule;
  XBMC->Log(LOG_DEBUG, "76::%s", __FUNCTION__);
  if (withTemplate)
  {
    // Base on template
    rule = NewFromTemplate(entry.epgInfo);
    // Override template with timer settings
    rule.SetStartOffset(rule.StartOffset() + entry.startOffset);
    rule.SetEndOffset(rule.EndOffset() + entry.endOffset);
    if (entry.dupMethod != GetRuleDupMethodDefaultId())
    {
      rule.SetDuplicateControlMethod(entry.dupMethod);
      rule.SetCheckDuplicatesInType(Myth::DI_InAll);
    }
    if (entry.priority != GetRulePriorityDefaultId())
      rule.SetPriority(entry.priority);
    if (entry.expiration != GetRuleExpirationDefaultId())
    {
      const MythScheduleManager::RuleExpiration& exr = GetRuleExpiration(entry.expiration);
      rule.SetAutoExpire(exr.autoExpire);
      rule.SetMaxEpisodes(exr.maxEpisodes);
      rule.SetNewExpiresOldRecord(exr.maxNewest);
    }
    if (entry.recordingGroup != RECGROUP_DFLT_ID)
      rule.SetRecordingGroup(GetRuleRecordingGroupName(entry.recordingGroup));
  }
  else
  {
    rule.SetCategory(entry.category);
    rule.SetStartOffset(entry.startOffset);
    rule.SetEndOffset(entry.endOffset);
    rule.SetDuplicateControlMethod(entry.dupMethod);
    rule.SetPriority(entry.priority);
    const MythScheduleManager::RuleExpiration& exr = GetRuleExpiration(entry.expiration);
    rule.SetAutoExpire(exr.autoExpire);
    rule.SetMaxEpisodes(exr.maxEpisodes);
    rule.SetNewExpiresOldRecord(exr.maxNewest);
    rule.SetRecordingGroup(GetRuleRecordingGroupName(entry.recordingGroup));
  }

  switch (entry.timerType)
  {
    case TIMER_TYPE_MANUAL_SEARCH:
    {
      if (entry.HasChannel() && entry.HasTimeSlot())
      {
        rule.SetType(Myth::RT_SingleRecord);
        rule.SetSearchType(Myth::ST_ManualSearch); // Using timeslot
        rule.SetChannelID(entry.chanid);
        rule.SetCallsign(entry.callsign);
        rule.SetStartTime(entry.startTime);
        rule.SetEndTime(entry.endTime);
        rule.SetTitle(entry.title);
        rule.SetDescription(entry.description);
        rule.SetInactive(entry.isInactive);
        return rule;
      }
      break;
    }

    case TIMER_TYPE_THIS_SHOWING:
    {
      if (!entry.epgInfo.IsNull())
      {
        rule.SetType(Myth::RT_SingleRecord);
        rule.SetSearchType(Myth::ST_NoSearch);
        rule.SetChannelID(entry.epgInfo.ChannelID());
        rule.SetStartTime(entry.epgInfo.StartTime());
        rule.SetEndTime(entry.epgInfo.EndTime());
        rule.SetTitle(entry.epgInfo.Title());
        rule.SetSubtitle(entry.epgInfo.Subtitle());
        rule.SetDescription(entry.description);
        rule.SetCallsign(entry.epgInfo.Callsign());
        rule.SetCategory(entry.epgInfo.Category());
        rule.SetProgramID(entry.epgInfo.ProgramID());
        rule.SetSeriesID(entry.epgInfo.SeriesID());
        rule.SetInactive(entry.isInactive);
        return rule;
      }
      break;
    }

    case TIMER_TYPE_RECORD_ONE:
    {
      if (!entry.epgInfo.IsNull())
      {
        rule.SetType(Myth::RT_OneRecord);
        rule.SetSearchType(Myth::ST_NoSearch);
        rule.SetChannelID(entry.epgInfo.ChannelID());
        rule.SetStartTime(entry.epgInfo.StartTime());
        rule.SetEndTime(entry.epgInfo.EndTime());
        rule.SetTitle(entry.epgInfo.Title());
        rule.SetSubtitle(entry.epgInfo.Subtitle());
        rule.SetDescription(entry.description);
        rule.SetCallsign(entry.epgInfo.Callsign());
        rule.SetCategory(entry.epgInfo.Category());
        rule.SetProgramID(entry.epgInfo.ProgramID());
        rule.SetSeriesID(entry.epgInfo.SeriesID());
        rule.SetInactive(entry.isInactive);
        return rule;
      }
      if (!entry.epgSearch.empty())
      {
        rule.SetType(Myth::RT_OneRecord);
        rule.SetSearchType(Myth::ST_TitleSearch); // Search title
        if (entry.HasChannel())
        {
          rule.SetFilter(Myth::FM_ThisChannel);
          rule.SetChannelID(entry.chanid);
          rule.SetCallsign(entry.callsign);
        }
        rule.SetTitle(entry.title);
        // Backend use the subtitle/description to find program by keywords or title
        rule.SetSubtitle("");
        rule.SetDescription(entry.epgSearch);
        rule.SetInactive(entry.isInactive);
        return rule;
      }
      break;
    }

    case TIMER_TYPE_RECORD_WEEKLY:
    {
      if (!entry.epgInfo.IsNull())
      {
        rule.SetType(Myth::RT_AllRecord);
        rule.SetFilter(Myth::FM_ThisChannel + Myth::FM_ThisDayAndTime);
        rule.SetSearchType(Myth::ST_NoSearch);
        rule.SetChannelID(entry.epgInfo.ChannelID());
        rule.SetStartTime(entry.epgInfo.StartTime());
        rule.SetEndTime(entry.epgInfo.EndTime());
        rule.SetTitle(entry.epgInfo.Title());
        rule.SetSubtitle(entry.epgInfo.Subtitle());
        rule.SetDescription(entry.description);
        rule.SetCallsign(entry.epgInfo.Callsign());
        rule.SetCategory(entry.epgInfo.Category());
        rule.SetProgramID(entry.epgInfo.ProgramID());
        rule.SetSeriesID(entry.epgInfo.SeriesID());
        rule.SetInactive(entry.isInactive);
        rule.SetDuplicateControlMethod(Myth::DM_CheckNone);
        return rule;
      }
      if (!entry.epgSearch.empty())
      {
        rule.SetType(Myth::RT_WeeklyRecord);
        rule.SetSearchType(Myth::ST_TitleSearch); // Search title
        if (entry.HasChannel())
        {
          rule.SetFilter(Myth::FM_ThisChannel);
          rule.SetChannelID(entry.chanid);
          rule.SetCallsign(entry.callsign);
        }
        rule.SetTitle(entry.title);
        // Backend use the subtitle/description to find program by keywords or title
        rule.SetSubtitle("");
        rule.SetDescription(entry.epgSearch);
        rule.SetInactive(entry.isInactive);
        rule.SetDuplicateControlMethod(Myth::DM_CheckNone);
        return rule;
      }
      if (entry.HasChannel() && entry.HasTimeSlot())
      {
        rule.SetType(Myth::RT_WeeklyRecord);
        rule.SetSearchType(Myth::ST_ManualSearch); // Using timeslot
        rule.SetChannelID(entry.chanid);
        rule.SetCallsign(entry.callsign);
        rule.SetStartTime(entry.startTime);
        rule.SetEndTime(entry.endTime);
        rule.SetTitle(entry.title);
        rule.SetDescription(entry.description);
        rule.SetInactive(entry.isInactive);
        rule.SetDuplicateControlMethod(Myth::DM_CheckNone);
        return rule;
      }
      break;
    }

    case TIMER_TYPE_RECORD_DAILY:
    {
      if (!entry.epgInfo.IsNull())
      {
        rule.SetType(Myth::RT_AllRecord);
        rule.SetFilter(Myth::FM_ThisChannel + Myth::FM_ThisTime);
        rule.SetSearchType(Myth::ST_NoSearch);
        rule.SetChannelID(entry.epgInfo.ChannelID());
        rule.SetStartTime(entry.epgInfo.StartTime());
        rule.SetEndTime(entry.epgInfo.EndTime());
        rule.SetTitle(entry.epgInfo.Title());
        rule.SetSubtitle(entry.epgInfo.Subtitle());
        rule.SetDescription(entry.description);
        rule.SetCallsign(entry.epgInfo.Callsign());
        rule.SetCategory(entry.epgInfo.Category());
        rule.SetProgramID(entry.epgInfo.ProgramID());
        rule.SetSeriesID(entry.epgInfo.SeriesID());
        rule.SetInactive(entry.isInactive);
        rule.SetDuplicateControlMethod(Myth::DM_CheckNone);
        return rule;
      }
      if (!entry.epgSearch.empty())
      {
        rule.SetType(Myth::RT_DailyRecord);
        rule.SetSearchType(Myth::ST_TitleSearch); // Search title
        if (entry.HasChannel())
        {
          rule.SetFilter(Myth::FM_ThisChannel);
          rule.SetChannelID(entry.chanid);
          rule.SetCallsign(entry.callsign);
        }
        rule.SetTitle(entry.title);
        // Backend use the subtitle/description to find program by keywords or title
        rule.SetSubtitle("");
        rule.SetDescription(entry.epgSearch);
        rule.SetInactive(entry.isInactive);
        return rule;
      }
      if (entry.HasChannel() && entry.HasTimeSlot())
      {
        rule.SetType(Myth::RT_DailyRecord);
        rule.SetSearchType(Myth::ST_ManualSearch); // Using timeslot
        rule.SetChannelID(entry.chanid);
        rule.SetCallsign(entry.callsign);
        rule.SetStartTime(entry.startTime);
        rule.SetEndTime(entry.endTime);
        rule.SetTitle(entry.title);
        rule.SetDescription(entry.description);
        rule.SetInactive(entry.isInactive);
        rule.SetDuplicateControlMethod(Myth::DM_CheckNone);
        return rule;
      }
      break;
    }

    case TIMER_TYPE_RECORD_ALL:
    {
      if (!entry.epgInfo.IsNull())
      {
        rule.SetType(Myth::RT_AllRecord);
        rule.SetFilter(Myth::FM_ThisChannel);
        rule.SetSearchType(Myth::ST_NoSearch);
        rule.SetChannelID(entry.epgInfo.ChannelID());
        rule.SetStartTime(entry.epgInfo.StartTime());
        rule.SetEndTime(entry.epgInfo.EndTime());
        rule.SetTitle(entry.epgInfo.Title());
        rule.SetSubtitle(entry.epgInfo.Subtitle());
        rule.SetDescription(entry.description);
        rule.SetCallsign(entry.epgInfo.Callsign());
        rule.SetCategory(entry.epgInfo.Category());
        rule.SetProgramID(entry.epgInfo.ProgramID());
        rule.SetSeriesID(entry.epgInfo.SeriesID());
        rule.SetInactive(entry.isInactive);
        return rule;
      }
      if (!entry.epgSearch.empty())
      {
        rule.SetType(Myth::RT_AllRecord);
        rule.SetSearchType(Myth::ST_TitleSearch); // Search title
        if (entry.HasChannel())
        {
          rule.SetFilter(Myth::FM_ThisChannel);
          rule.SetChannelID(entry.chanid);
          rule.SetCallsign(entry.callsign);
        }
        rule.SetTitle(entry.title);
        // Backend use the subtitle/description to find program by keywords or title
        rule.SetSubtitle("");
        rule.SetDescription(entry.epgSearch);
        rule.SetInactive(entry.isInactive);
        return rule;
      }
      break;
    }

    case TIMER_TYPE_RECORD_SERIES:
    {
      if (!entry.epgInfo.IsNull())
      {
        rule.SetType(Myth::RT_AllRecord);
        rule.SetFilter(Myth::FM_ThisChannel | Myth::FM_ThisSeries);
        rule.SetSearchType(Myth::ST_NoSearch);
        rule.SetChannelID(entry.epgInfo.ChannelID());
        rule.SetStartTime(entry.epgInfo.StartTime());
        rule.SetEndTime(entry.epgInfo.EndTime());
        rule.SetTitle(entry.epgInfo.Title());
        rule.SetSubtitle(entry.epgInfo.Subtitle());
        rule.SetDescription(entry.description);
        rule.SetCallsign(entry.epgInfo.Callsign());
        rule.SetCategory(entry.epgInfo.Category());
        rule.SetProgramID(entry.epgInfo.ProgramID());
        rule.SetSeriesID(entry.epgInfo.SeriesID());
        rule.SetInactive(entry.isInactive);
        return rule;
      }
      break;
    }

    case TIMER_TYPE_SEARCH_KEYWORD:
    {
      if (!entry.epgSearch.empty())
      {
        rule.SetType(Myth::RT_AllRecord);
        rule.SetSearchType(Myth::ST_KeywordSearch); // Search keyword
        if (entry.HasChannel())
        {
          rule.SetFilter(Myth::FM_ThisChannel);
          rule.SetChannelID(entry.chanid);
          rule.SetCallsign(entry.callsign);
        }
        rule.SetTitle(entry.title);
        // Backend use the subtitle/description to find program by keywords or title
        rule.SetSubtitle("");
        rule.SetDescription(entry.epgSearch);
        rule.SetInactive(entry.isInactive);
        return rule;
      }
      break;
    }

    case TIMER_TYPE_SEARCH_PEOPLE:
    {
      if (!entry.epgSearch.empty())
      {
        rule.SetType(Myth::RT_AllRecord);
        rule.SetSearchType(Myth::ST_PeopleSearch); // Search people
        if (entry.HasChannel())
        {
          rule.SetFilter(Myth::FM_ThisChannel);
          rule.SetChannelID(entry.chanid);
          rule.SetCallsign(entry.callsign);
        }
        rule.SetTitle(entry.title);
        // Backend use the subtitle/description to find program by keywords or title
        rule.SetSubtitle("");
        rule.SetDescription(entry.epgSearch);
        rule.SetInactive(entry.isInactive);
        return rule;
      }
      break;
    }

    case TIMER_TYPE_DONT_RECORD:
    case TIMER_TYPE_OVERRIDE:
    case TIMER_TYPE_UPCOMING:
    case TIMER_TYPE_UPCOMING_MANUAL:
    case TIMER_TYPE_ZOMBIE:
      // any type
      rule.SetType(Myth::RT_NotRecording);
      rule.SetChannelID(entry.chanid);
      rule.SetCallsign(entry.callsign);
      rule.SetStartTime(entry.startTime);
      rule.SetEndTime(entry.endTime);
      rule.SetTitle(entry.title);
      rule.SetDescription(entry.description);
      rule.SetInactive(entry.isInactive);
      return rule;

    default:
      break;
  }
  rule.SetType(Myth::RT_UNKNOWN);
  XBMC->Log(LOG_ERROR, "76::%s: Invalid timer %u: TYPE=%d CHANID=%u SIGN=%s ST=%u ET=%u", __FUNCTION__, entry.entryIndex,
          entry.timerType, entry.chanid, entry.callsign.c_str(), (unsigned)entry.startTime, (unsigned)entry.endTime);
  return rule;
}
