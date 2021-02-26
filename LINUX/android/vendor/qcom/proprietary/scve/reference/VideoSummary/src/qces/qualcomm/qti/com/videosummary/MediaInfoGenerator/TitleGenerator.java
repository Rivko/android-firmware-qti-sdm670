/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.MediaInfoGenerator;

import android.content.Context;
import android.util.Pair;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;

import qces.qualcomm.qti.com.videosummary.Util.CalendarHelper;
import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsModel;

/**
 * Created by mithun on 9/9/15.
 */
public class TitleGenerator {
    ArrayList<MediaInfo> mediaInfoList;
    int numberOfVideos;
    Context context;
    long createDate;

    String title = "untitled";
    int titleDay;
    String titleDayOfWeek;
    int titleWeek;
    String titleMonth;
    int titleYear;
    String titleLocation;
    String subTitle = "";

    DateCategory dateCategory;
    LocationCategory locationCategory;

    HashMap<String, Integer> cityMap = new HashMap<>();
    HashMap<String, Integer> stateMap = new HashMap<>();

    HashMap<Integer, Integer> dayMap = new HashMap<>();
    HashMap<Integer, Integer> weekMap = new HashMap<>();
    HashMap<Integer, Integer> monthMap = new HashMap<>();
    HashMap<Integer, Integer> yearMap = new HashMap<>();


    public enum DateCategory {
        UNKNOWN,
        SAME_DAY,
        SAME_WEEK,
        SAME_MONTH,
        SAME_YEAR,
    }


    public enum LocationCategory {
        UNKNOWN,
        SAME_CITY,
        SAME_STATE
    }

    public TitleGenerator(Context context, ArrayList<MediaInfo> list, HighlightsModel.SummaryType summaryType) {
        this.context = context;
        this.mediaInfoList = list;
        dateCategory = DateCategory.UNKNOWN;
        locationCategory = LocationCategory.UNKNOWN;
        if (list != null) {
            numberOfVideos = list.size();
        }

        if (numberOfVideos > 0) {
            computeTitle(summaryType);
        }

    }

    public String getTitle() {
        return title;
    }

    public String getSubTitle() {
        return subTitle;
    }

    void computeTitle(HighlightsModel.SummaryType summaryType) {
        generateHashMaps();
        setDateCategory();
        setLocationCategory();
        switch (summaryType) {
            case HIGHLIGHTS:
                frameHighlightsTitle();
                break;
            case TIMELAPSE:
                frameTimelapseTitle();
                break;
            default:
                throw new UnsupportedOperationException("Summary mode unknown" + summaryType.toString());
        }

    }

    void generateHashMaps() {

        for (int i = 0; i < numberOfVideos; i++) {
            MediaInfo info = mediaInfoList.get(i);
            /**
             * determine summarized location info here and ultimately the title
             */
            Pair<String, String> cityState = Utils.getLocationInfo(context, info.latitude, info.longitude);
            String city = cityState.first;
            String state = cityState.second;
            int count = 0;
            if (cityMap.get(city) != null) {
                count = cityMap.get(city);
                cityMap.put(city, ++count);
            } else {
                cityMap.put(city, 1);
            }
            if (stateMap.get(state) != null) {
                count = stateMap.get(state);
                stateMap.put(state, ++count);
            } else {
                stateMap.put(state, 1);
            }

            createDate = info.createdDate;//record atlease one date for calculating subTitle in generateSubTitleWeek()
            //aggerigate date, month and year info
            Calendar calendar = Calendar.getInstance();
            calendar.setTimeInMillis(info.createdDate);
            int day = calendar.get(Calendar.DATE);
            int week = calendar.get(Calendar.WEEK_OF_YEAR);
            int month = calendar.get(Calendar.MONTH);
            int year = calendar.get(Calendar.YEAR);

            if (dayMap.get(day) != null) {
                count = dayMap.get(day);
                dayMap.put(day, ++count);
            } else {
                dayMap.put(day, 1);
            }
            if (weekMap.get(week) != null) {
                count = weekMap.get(week);
                weekMap.put(week, ++count);
            } else {
                weekMap.put(week, 1);
            }
            if (monthMap.get(month) != null) {
                count = monthMap.get(month);
                monthMap.put(month, ++count);
            } else {
                monthMap.put(month, 1);
            }
            if (yearMap.get(year) != null) {
                count = yearMap.get(year);
                yearMap.put(year, ++count);
            } else {
                yearMap.put(year, 1);
            }
        }

    }

    void setDateCategory() {
        int finalYear = getMax(yearMap);
        int finalMonth = getMax(monthMap);
        int finalWeek = getMax(weekMap);
        int finalDay = getMax(dayMap);

        Calendar calendar = Calendar.getInstance();
        calendar.set(finalYear, finalMonth, finalDay);

        if (yearMap.get(finalYear) == numberOfVideos) {
            //all in same year
            dateCategory = DateCategory.SAME_YEAR;
            titleYear = finalYear;
            if (monthMap.get(finalMonth) == numberOfVideos) {
                //all in same month
                dateCategory = DateCategory.SAME_MONTH;
                titleMonth = CalendarHelper.getMonthFromCalendar(calendar);
                if (weekMap.get(finalWeek) == numberOfVideos) {
                    //all in same week
                    dateCategory = DateCategory.SAME_DAY.SAME_WEEK;
                    titleWeek = calendar.get(Calendar.WEEK_OF_MONTH);
                    if (dayMap.get(finalDay) == numberOfVideos) {
                        //all in same day
                        dateCategory = DateCategory.SAME_DAY;
                        titleDay = finalDay;
                        titleDayOfWeek = CalendarHelper.getDayOfWeekFromCalendar(calendar);
                    }
                }
            }
        }
    }

    void setLocationCategory() {
        String city = getMax(cityMap);
        String state = getMax(stateMap);

        if (stateMap.get(state) >= (0.5 * numberOfVideos) && state != null) {
            //more then 50% are from same state
            locationCategory = LocationCategory.SAME_STATE;
            titleLocation = state;
            if (cityMap.get(city) >= (0.5 * numberOfVideos) && city != null) {
                //more then 50% are from same city & state
                locationCategory = LocationCategory.SAME_CITY;
                titleLocation = city + ", " + state;
            }
        }
    }

    <T> T getMax(HashMap<T, Integer> map) {
        T maxKey = null;
        int maxCount = 0;
        for (HashMap.Entry<T, Integer> entry : map.entrySet()) {
            System.out.printf("Key : %s and Value: %s %n", entry.getKey(), entry.getValue());
            T key = (T) entry.getKey();
            int count = entry.getValue();
            if (maxCount < count) {
                maxCount = count;
                maxKey = key;
            }
        }
        return (T) maxKey;
    }


    void frameHighlightsTitle() {
        switch (dateCategory) {
            case SAME_DAY:
                if (locationCategory == LocationCategory.SAME_CITY || locationCategory == LocationCategory.SAME_STATE) {
                    title = titleDayOfWeek + " in " + titleLocation;
                } else {
                    //title = titleDayOfWeek + " " + titleMonth + " " + titleDay + "" + ordinal(titleDay) + ", " + titleYear;
                    title = titleDayOfWeek + "'s Highlights";
                }
                subTitle = generateSubTitleDay();
                break;
            case SAME_WEEK:
                if (locationCategory == LocationCategory.SAME_CITY || locationCategory == LocationCategory.SAME_STATE) {
                    title = titleWeek + ordinal(titleWeek) + " week of " + titleMonth + " in " + titleLocation;
                } else {
                    //title = titleWeek + ordinal(titleWeek) + " week of " + titleMonth + ", " + titleYear;
                    title = "Last Week's Highlights";
                }
                subTitle = generateSubTitleWeek();
                break;
            case SAME_MONTH:
                if (locationCategory == LocationCategory.SAME_CITY || locationCategory == LocationCategory.SAME_STATE) {
                    title = titleMonth + " in " + titleLocation;
                } else {
                    //title = titleMonth + ", " + titleYear;
                    title = titleMonth + "'s Highlights";
                }
                subTitle = generateSubTitleMonth();
                break;
            case SAME_YEAR:
                if (locationCategory == LocationCategory.SAME_CITY || locationCategory == LocationCategory.SAME_STATE) {
                    title = "Year " + titleYear + " in " + titleLocation;
                } else {
                    title = "Year " + titleYear + " summary";
                }
                break;
            default:
                title = "Highlights";

        }
    }

    void frameTimelapseTitle() {
        switch (dateCategory) {
            case SAME_DAY:
                if (locationCategory == LocationCategory.SAME_CITY || locationCategory == LocationCategory.SAME_STATE) {
                    title = "Timelapse from " + titleLocation;
                } else {
                    //title = "Timelapse from " + titleDayOfWeek + " " + titleMonth + " " + titleDay + "" + ordinal(titleDay) + ", " + titleYear;
                    title = "Timelapse from " + titleDayOfWeek;
                }
                subTitle = generateSubTitleDay();
                break;
            case SAME_WEEK:
               /* if (locationCategory == LocationCategory.SAME_CITY || locationCategory == LocationCategory.SAME_STATE) {
                    title = titleWeek + ordinal(titleWeek) + " week of " + titleMonth + " in " + titleLocation;
                } else {
                    title = titleWeek + ordinal(titleWeek) + " week of " + titleMonth + ", " + titleYear;
                }
                subTitle = generateSubTitleWeek(createDate);
                break;*/
            case SAME_MONTH:
                /*if (locationCategory == LocationCategory.SAME_CITY || locationCategory == LocationCategory.SAME_STATE) {
                    title = "Timelapse from " + titleLocation;
                } else {
                    title = "Timelapse";
                }
                subTitle = generateSubTitleMonth();
                break;*/
            case SAME_YEAR:
                if (locationCategory == LocationCategory.SAME_CITY || locationCategory == LocationCategory.SAME_STATE) {
                    title = "Timelapse from " + titleLocation;
                } else {
                    title = "Timelapse";
                }
                break;
            default:
                title = "Timelapse";

        }
    }

    String generateSubTitleDay() {
        String subTitle = titleMonth + " " + titleDay + ", " + titleYear;
        return subTitle;
    }

    String generateSubTitleWeek() {
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(createDate);

        String startDate = Integer.toString(CalendarHelper.getStartMondayDateOfWeekFromCalendar(calendar));
        String endDate = Integer.toString(CalendarHelper.getEndSundayDateOfWeekFromCalendar(calendar));

        String subTitle = titleMonth + " " + startDate + "-" + endDate + ", " + titleYear;
        return subTitle;
    }

    String generateSubTitleMonth() {
        String subTitle = titleMonth + ", " + titleYear;
        return subTitle;
    }

    String ordinal(int num) {
        String[] ordinals = new String[]{"th", "st", "nd", "rd", "th", "th", "th", "th", "th", "th"};
        switch (num % 100) {
            case 11:
            case 12:
            case 13:
                return "th";
            default:
                return ordinals[num % 10];

        }
    }

}
